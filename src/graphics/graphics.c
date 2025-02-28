
#include "graphics.h"

#include "base/base.h"
#include "os/os.h"
#include <unistd.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>

const uint16 screenWidth = 1080;
const uint16 screenHeight = 720;

typedef struct GWindow {
    uint32 width, height;
    uint32 black, white;

    Display *display;
    int screen;
    Window window;
    GC gc;

    XEvent event;
    KeySym key;

    Atom wm_delete_window;

    bool32 alive;
} GWindow;

void GWindowInit(GWindow *gw)
{

    gw->alive = true;
    gw->event = (XEvent){0};
    gw->key = (KeySym){0};
    gw->width = screenWidth;
    gw->height = screenHeight;

    gw->display = XOpenDisplay((char *)NULL);
    gw->screen = XDefaultScreen(gw->display);

    Display *display = gw->display;
    int screen = gw->screen;

    gw->black = BlackPixel(display, screen);
    gw->white = WhitePixel(display, screen);

    gw->window = XCreateSimpleWindow(display, XDefaultRootWindow(display),
            0, 0, gw->width, gw->height, 5, gw->white, gw->black);

    Window window = gw->window;

    XSetStandardProperties(display, window, "x11 window", "hi", None, NULL, 0, NULL);

    XSelectInput(display, window, ExposureMask | ButtonPressMask | KeyPressMask |
                                  StructureNotifyMask);

    XWindowAttributes attr = {0};
    XGetWindowAttributes(display, window, &attr);

    gw->gc = XCreateGC(display, window, 0, NULL);

    XClearWindow(display, window);

    XSetBackground(display, gw->gc, gw->white);
    XSetBackground(display, gw->gc, gw->black);

    // NOTE(liam): like XMapWindow but also raises to top of stack.
    XMapRaised(display, window);

    gw->wm_delete_window = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, window, &gw->wm_delete_window, 1);
}

void GWindowClose(GWindow *gw)
{
    XFreeGC(gw->display, gw->gc);
    XDestroyWindow(gw->display, gw->window);
    XCloseDisplay(gw->display);
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

    /*XDrawPoint(display, window, gc, 300, 300);*/
    /*XDrawLine(display, window, gc, 20, 20, 40, 100);*/
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
    else if (gw->event.type == KeyPress && XLookupString(&gw->event.xkey, text,
                255, &gw->key, 0) == 1)
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

    GWindowInit(&gw);

    while (gw.alive)
    {
        GWindowEvent(&gw);
    }

    GWindowClose(&gw);
    return 0;
}
