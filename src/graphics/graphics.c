
#include "graphics.h"

#include "base/base.h"
#include "os/os.h"
#include <unistd.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>

const uint16 screenWidth = 1080;
const uint16 screenHeight = 720;


typedef struct Color {
    uint8 r, g, b;
    uint8 a;
} Color;

Color GColor()
{
    // returns white.
    return (Color){ 255, 0, 255, 255 };
}

// used for screen space nav
typedef struct Vector2u {
    uint32 x, y;
} Vector2u;

typedef struct GWindow {
    uint32 width, height;
    uint32 black, white;

    uint32 mouseX, mouseY;

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
    /*GLXFBConfig *fbconfigs, fbconfig;*/
    /*int numfbconfigs;*/
    /*GLXWindow glx_window;*/

    Atom wm_delete_window;
    bool32 alive;
} GWindow;


uint32 GColorConvert(Color c)
{
    uint32 color = (c.r << 16) | (c.g << 8) | c.b;

    return color;
}

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

    XSetForeground(display, gw->gc, gw->white);
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

void GWindowDrawPixel(GWindow *gw, Vector2u pos, uint32 color_pixel)
{
    XSetForeground(gw->display, gw->gc, color_pixel);
    XDrawPoint(gw->display, gw->window, gw->gc, (int)pos.x, (int)pos.y);
}

void GWindowDrawLine(GWindow *gw, Vector2u a, Vector2u b, uint32 color_pixel)
{
    XSetForeground(gw->display, gw->gc, color_pixel);
    XDrawLine(gw->display, gw->window, gw->gc, (int)a.x, (int)a.y, (int)b.x, (int)b.y);
}

void GWindowDrawTriangle(GWindow *gw, Vector2u a, Vector2u b, Vector2u c, uint32 color_pixel)
{
    GWindowDrawLine(gw, a, b, color_pixel);
    GWindowDrawLine(gw, b, c, color_pixel);
    GWindowDrawLine(gw, c, a, color_pixel);
}

void GWindowClear(GWindow *gw)
{
    XSetForeground(gw->display, gw->gc, gw->black);
    XFillRectangle(gw->display, gw->window, gw->gc, 0, 0, gw->width, gw->height);
}

void GWindowDraw(GWindow *gw)
{
    Color white = GColor();

    GWindowDrawTriangle(gw, (Vector2u){gw->mouseX, gw->mouseY}, (Vector2u){300, 200}, (Vector2u){600, 100}, GColorConvert(white));
}

void GWindowEvent(GWindow *gw)
{
    gw->event = (XEvent){0};
    XNextEvent(gw->display, &gw->event);

    switch (gw->event.type)
    {
        case ClientMessage:
        {
            if ((Atom)gw->event.xclient.data.l[0] == gw->wm_delete_window)
            {
                gw->alive = false;
            }
        } break;
        case Expose:
        {
            if (gw->event.xexpose.count == 0)
            {
                GWindowDraw(gw);
            }
        } break;
        case ConfigureNotify:
        {
            XConfigureEvent xce = gw->event.xconfigure;

            if ((xce.width <= screenWidth && xce.width >= 400) &&
                    (xce.height <= screenHeight && xce.height >= 300))
            {
                gw->width = xce.width;
                gw->height = xce.height;
                GWindowDraw(gw);
            }
        } break;
        case KeyPress:
        {
            char key = XLookupKeysym(&gw->event.xkey, 0);
            switch (key)
            {
                case '\x1b':
                case 'q':
                    {
                        gw->alive = false;
                    } __attribute__((fallthrough));
                default:
                    {
                        printf("Pressed %c!\n", key);
                    } break;
            }
        } break;
        case ButtonPress:
        {
            printf("Mouse pressed at (%i, %i)\n",
                    gw->event.xbutton.x, gw->event.xbutton.y);
            gw->mouseX = gw->event.xbutton.x;
            gw->mouseY = gw->event.xbutton.y;
        } break;
        default:
        {} break;
    }
}

int main(void)
{
    struct GWindow gw = {0};

    GWindowInit(&gw);

    while (gw.alive)
    {
        GWindowEvent(&gw);

        GWindowClear(&gw);
        GWindowDraw(&gw);
    }

    GWindowFree(&gw);
    return 0;
}
