
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
    Window root, window;
    GC gc;
    /*Visual *visual;*/
    XVisualInfo *visual;

    XFontStruct *font;

    XEvent event;
    KeySym key;

    Colormap colormap;

    uint32 fontHeight;
    uint32 screenRows, screenCols;

    // GL
    GLXFBConfig *fbconfigs, fbconfig;
    int numfbconfigs;
    GLXWindow glx_window;

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

void GWindowGLInit(GWindow *gw)
{
	XSetWindowAttributes attr = {0,};

    gw->alive = true;
    gw->event = (XEvent){0};
    gw->key = (KeySym){0};
    gw->width = screenWidth;
    gw->height = screenHeight;

    gw->display = XOpenDisplay((char *)NULL);
    gw->screen = DefaultScreen(gw->display);
    gw->root = RootWindow(gw->display, gw->screen);

    int visdata[] = {
        GLX_RENDER_TYPE, GLX_RGBA_BIT,
        GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
        GLX_DOUBLEBUFFER, True,
        GLX_RED_SIZE, 8,
        GLX_GREEN_SIZE, 8,
        GLX_BLUE_SIZE, 8,
        GLX_ALPHA_SIZE, 8,
        GLX_DEPTH_SIZE, 16,
        None
    };

    gw->fbconfigs = glXChooseFBConfig(gw->display, gw->screen, visdata, &gw->numfbconfigs);
    gw->fbconfig = 0;

    for (uint32 i = 0; i < gw->numfbconfigs; i++)
    {
        gw->visual = (XVisualInfo *)glXGetVisualFromFBConfig(gw->display, gw->fbconfigs[i]);

        if (!visual)
        {
            continue;
        }

        pict_format = XRenderFindVisualFormat(gw->display, gw->visual->visual);
        if (!pict_format)
        {
            continue;
        }

        gw->fbconfig = gw->fbconfigs[i];
        if (pict_format->direct.alphaMask > 0)
        {
            break;
        }
        XFree(visual);
    }

    if (!fbconfig)
    {

    }

    // describe_fbconfig

    gw->colormap = XCreateColormap(gw->display, gw->visual->visual, AllocNone);

    attr.colormap = gw->colormap;
    attr.background_pixmap = None;
    atrr.border_pixmap = None;
    attr.border_pixel = 0;
    attr.event_mask = ExposureMask | ButtonPressMask | EnterWindowMask |
                      LeaveWindowMask |KeyPressMask | StructureNotifyMask;

    int x = gw->width / 2;
    int y = gw->height / 2;
    int attr_mask = CWColormap | CWBorderPixel | CWEventMask;

    gw->window = XCreateWindow(
        gw->display,
        gw->root,
        x, y,
        gw->width, gw->height,
        gw->visual->depth,
        InputOutput,
        gw->visual->visual,
        attr_mask,
        &attr
    );

    int glXattr[] = { None };
    gw->glx_window = glXCreateWindow(gw->display, gw->fbconfig, gw->window, glXattr);

    XEvent event;

	XSizeHints hints;
    hints.x = x;
    hints.y = y;
    hints.width = gw->width;
    hints.height = gw->height;
    hints.flags = USPosition | USSize;

	XWMHints *startup_state = XAllocWMHints();
    startup_state->initial_state = NormalState;
    startup_state->flags = StateHint;

    XSetWMProperties(gw->display, gw->window, NULL, NULL, NULL, 0,
                     &hints, startup_state, NULL);

    XFree(startup_state);

    XMapRaised(gw->display, gw->window);
    XIfEvent(gw->display, &gw->event, WaitForMapNotify, (char *)&gw->window);

    if ((gw->wm_delete_window = XInternAtom(gw->display, "WM_DELETE_WINDOW", 0)) != None) {
		XSetWMProtocols(gw->display, gw->window, &gw->wm_delete_window, 1);
	}
}

void GWindowInit(GWindow *gw)
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
    if (gw->gc)
    {
        XFreeGC(gw->display, gw->gc);
    }
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
