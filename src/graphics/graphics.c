
#include "graphics.h"

#include "base/base.h"
#include "os/os.h"
#include <unistd.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>

#include <GL/gl.h>
#include <GL/glx.h>

const uint16 screenWidth = 1080;
const uint16 screenHeight = 720;

typedef GLXContext (*glXCreateContextAttribsARBProc)(Display*, GLXFBConfig, GLXContext, bool32, const int*);

typedef struct GWindow {
    uint32 width, height;
    /*uint32 black, white;*/

    Display *display;
    int screen;
    Window window;
    GC gc;
    Visual *visual;

    XFontStruct *font;

    XEvent event;
    KeySym key;

    Colormap colormap;

    uint32 fontHeight;
    uint32 screenRows, screenCols;

    Atom wm_delete_window;
    bool32 alive;
} GWindow;

typedef struct GConfig {

} GConfig;

typedef struct GLConfig {

} GLConfig;

void GWindowFont(GWindow *gw, char *font_name)
{
    if (gw->font)
    {
        XFreeFont(gw->display, gw->font);
    }

    gw->font = XLoadQueryFont(gw->display, font_name);
    if (gw->font == NULL)
    {
        fprintf(stderr, "XLoadQueryFont: failed to load font '%s'\n", font_name);
    }
    XSetFont(gw->display, gw->gc, gw->font->fid);

    gw->fontHeight = gw->font->ascent + gw->font->descent;
}

void GWindowInit(GWindow *gw, bool32 glx_enabled)
{

    gw->alive = true;
    gw->event = (XEvent){0};
    gw->key = (KeySym){0};
    gw->width = screenWidth;
    gw->height = screenHeight;

    gw->display = XOpenDisplay((char *)NULL);
    gw->screen = DefaultScreen(gw->display);

    Display *display = gw->display;
    int screen = gw->screen;

    if (glx_enabled)
    {
        int visual_attribs[] = {
            GLX_X_RENDERABLE    , True,
            GLX_DRAWABLE_TYPE   , GLX_WINDOW_BIT,
            GLX_RENDER_TYPE     , GLX_RGBA_BIT,
            GLX_X_VISUAL_TYPE   , GLX_TRUE_COLOR,
            glx_red_size        , 8,
            glx_green_size      , 8,
            glx_blue_size       , 8,
            glx_alpha_size      , 8,
            glx_depth_size      , 24,
            glx_stencil_size    , 8,
            glx_doublebuffer    , true,
            //glx_sample_buffers  , 1,
            //glx_samples         , 4,
            none
        };

        int glx_major, glx_minor;
        if ( !glxQueryVersion( display, &glx_major, &glx_minor ) ||
                ( ( glx_major == 1 ) && ( glx_minor < 3 ) ) || ( glx_major < 1 ) )
        {
            printf("invalid glx version");
            return false;
        }

        int fbcount;
        GLXFBConfig *fbc = glxChooseFBConfig(display, screen, visual_attribs, &fbcount);
        if (!fbc)
        {
            printf( "failed to retrieve a framebuffer config\n" );
            return false;
        }

        int best_fbc = -1, worst_fbc = -1, best_num_samp = -1, worst_num_samp = 999;

        for (int i = 0; i < fbcount; i++)
        {
            XVisualInfo *vi = glXGetVisualFromFBConfig(display, fbc[i]);
            if (vi)
            {
            int samp_buf, samples;
            glXGetFBConfigAttrib(display, fbc[i], GLX_SAMPLE_BUFFERS, &samp_buf);
            glXGetFBConfigAttrib(display, fbc[i], GLX_SAMPLES, &samples);

            if (best_fbc < 0 || samp_buf && samples > best_num_samp)
            {
                best_fbc = i;
                best_num_samp = samples;
            }
            if (worst_fbc < 0 || !samp_buf || samples < worst_num_samp)
            {
                worst_fbc = i;
                worst_num_samp = samples;
            }
        }
        XFree(vi);
    }

    GLXFBConfig bestfbc = fbc[best_fbc];

    XFree(fbc);

    XVisualInfo *vi = glXGetVisualFromFBConfig(display, bestfbc);

    // creating colormap
    XSetWindoAattributes swa;
    gw->colormap;
    swa.colormap = gw->cmap = xcreatecolormap(display,
                                          rootwindow(display, vi->screen),
                                          vi->visual, allocnone);
    swa.background_pixmap = none;
    swa.border_pixel= 0;
    swa.event_mask = structurenotifymask;

    gw->window = XCreateWindow(display, RootWindow(display, vi->screen),
                               0, 0, 100, 100, 0, vi->depth, InputOutput,
                               vi->visual,
                               CWBorderPixel | CWColormap | CWEventMask, &swa);

    Window window = gw->window;

    XFree(vi);

    XStoreName(display, window, "x11+gl window");

    XMapRaised(display, window);

    const char *glxExts = glXQueryExtensionsString(display, screen);

    glXCreateContextAttribsARBPRoc glXCreateContextAttribsARB = 0;
    glXCreateContextAttribsARB  = (glXCreateContextAttribsARBPRoc)
        glXGetProcAddressARB((const GLubyte *) "glXCreateContextAttribsARB");

    GLXContext ctx = 0;

    int context_attribs[] = {
        GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
        GLX_CONTEXT_MINOR_VERSION_ARB, 0,
        None
    };

    ctx = GlXCreateContextAttribsARB(display, bestFbc, 0, True, context_attribs);
    XSync(display, False);

    glXMakeCurrent(display, win, ctx);

    }
    else
    {
        gw->black = BlackPixel(display, screen);
        gw->white = WhitePixel(display, screen);

        gw->window = XCreateSimpleWindow(display, DefaultRootWindow(display),
                0, 0, gw->width, gw->height, 2, gw->white, gw->black);

        Window window = gw->window;

        XSetStandardProperties(display, window, "x11 window", "hi", None, NULL, 0, NULL);
        /*XSetWMIconName(display, window, ...);*/

        XSelectInput(display, window, ExposureMask | ButtonPressMask |
                KeyPressMask | StructureNotifyMask);

        XWindowAttributes attr = {0};
        XGetWindowAttributes(display, window, &attr);

        gw->gc = XCreateGC(display, window, 0, NULL);

        gw->wm_delete_window = XInternAtom(display, "WM_DELETE_WINDOW", False);
        XSetWMProtocols(display, window, &gw->wm_delete_window, 1);

        GWindowFont(gw, "6x13");


        XSizeHints *win_size_hints = XAllocSizeHints();
        if (!win_size_hints)
        {
            fprintf(stderr, "XAllocSizeHints unable to allocate memory\n");
            return;
        }

        // NOTE(liam): preferred window sizing
        win_size_hints->flags = PSize | PMinSize;

        win_size_hints->min_width = 300;
        win_size_hints->min_height = 200;
        win_size_hints->base_width = screenWidth;
        win_size_hints->base_height = screenHeight;

        XSetWMNormalHints(display, window, win_size_hints);
        XFree(win_size_hints);

        gw->colormap = DefaultColormap(display, screen);

        XClearWindow(display, window);

        XSetBackground(display, gw->gc, gw->white);
        XSetBackground(display, gw->gc, gw->black);

        // NOTE(liam): like XMapWindow but also raises to top of stack.
        XMapRaised(display, window);
        XFlush(display);
    }

}

void GWindowFree(GWindow *gw)
{
    // NOTE(liam): gw should not be referred to after free,
    // unless GWindowInit is called on it again.
    if (gw->font)
    {
        XFreeFont(gw->display, gw->font);
    }
    if (gw->colormap)
    {
        XFreeColormap(gw->display, gw->colormap);
    }
    XFreeGC(gw->display, gw->gc);
    XDestroyWindow(gw->display, gw->window);
    XCloseDisplay(gw->display);
}

void GWindowWrite(GWindow *gw, char *s, memory_index len)
{
    static int x;
    static int y = 0;
    int string_width;

    string_width = XTextWidth(gw->font, s, len);

    x = string_width;

    if (x == screenWidth)
    {
        x = 0;
    }
    x += 10;
    if (y == screenHeight)
    {
        y = string_width;
    }
    y += 10;
    XDrawString(gw->display, gw->window, gw->gc, x, y, s, len);
}

void GWindowDraw(GWindow *gw)
{
    Window window = gw->window;
    Display *display = gw->display;
    GC gc = gw->gc;

    XClearWindow(display, window);
    // TODO(liam): add draws here.

    XSetForeground(display, gc, gw->white);
    XSetBackground(display, gc, gw->black);

    XSetFillStyle(display, gc, FillSolid);

    XSetLineAttributes(display, gc, 2, LineSolid, CapRound, JoinRound);

    GWindowWrite(gw, "howdy", 5);
    XDrawPoint(display, window, gc, 300, 300);
    XDrawLine(display, window, gc, 20, 20, 40, 100);
}

void GWindowEvent(GWindow *gw)
{
    char text[255];

    XNextEvent(gw->display, &gw->event);
    if (gw->event.type == ClientMessage)
    {
        if ((Atom)gw->event.xclient.data.l[0] == gw->wm_delete_window)
        {
            gw->alive = false;
        }
    }
    else if (gw->event.type == Expose && gw->event.xexpose.count == 0)
    {
        /*redraw();*/
        GWindowDraw(gw);
    }
    else if (gw->event.type == ConfigureNotify)
    {
        XConfigureEvent xce = gw->event.xconfigure;

        if ((xce.width <= screenWidth && xce.width >= 400) &&
            (xce.height <= screenHeight && xce.height >= 300))
        {
            gw->width = xce.width;
            gw->height = xce.height;
            GWindowDraw(gw);
        }
    }
    else if (gw->event.type == KeyPress &&
             XLookupString(&gw->event.xkey, text, 255, &gw->key, 0) == 1)
    {
        // NOTE(liam): KEYPRESS
        if (text[0] == 'q' || text[0] == '\x1b')
        {
            gw->alive = false;
        }
        printf("Pressed %c!\n", text[0]);
    }
    if (gw->event.type == ButtonPress)
    {
        // NOTE(liam): MOUSE
        printf("Mouse pressed at (%i, %i)\n",
               gw->event.xbutton.x, gw->event.xbutton.y);
    }
}

int main(void)
{
    struct GWindow gw = {0};

    GWindowInit(&gw, true);

    while (gw.alive)
    {
        GWindowDraw(&gw);
        GWindowEvent(&gw);
    }

    GWindowFree(&gw);
    return 0;
}
