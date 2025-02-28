
#include "graphics.h"

#include "base/base.h"
#include "os/os.h"
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

const uint16 screenWidth = 1080;
const uint16 screenHeight = 720;

void PrintDisplayInfo(Display *display, int screen)
{
    int screen_num, display_width, display_height, width, height;

    /* get screen size from display structure macro */
    screen_num = DefaultScreen(display);
    display_width = DisplayWidth(display, screen_num);
    display_height = DisplayHeight(display, screen_num);

    fprintf(stderr, "DisplayString: %s\n", DisplayString(display));
    fprintf(stderr, "default screen index: %d\n", screen_num);
    fprintf(stderr, "display width: %d\n", display_width);
    fprintf(stderr, "display height: %d\n", display_height);
}

int main(void)
{
    Display *display;
    int screen;
    Window root;
    GC gc;

    Window window;
    Visual *visual = CopyFromParent;
    int x, y;
    char *msg = "hello world!";

    int border_width = 2;
    XSetWindowAttributes attr;
    uint32 attribute_mask = CWEventMask | CWBackPixel | CWBorderPixel;
    uint32 event_mask = ExposureMask | ButtonPressMask | KeyPressMask;


    int done = 0;
    XEvent event;


    display = XOpenDisplay((char *)NULL);
    if (display == (Display *) NULL)
    {
        fprintf(stderr, "unable to connect to X server [%s]\n", XDisplayName((char *) NULL));
    }

    screen = DefaultScreen(display);
    root = RootWindow(display, screen);
    gc = DefaultGC(display, screen);
    PrintDisplayInfo(display, screen);

    x = y = 150;
    attr.event_mask = event_mask;
    attr.border_pixel = BlackPixel(display, screen);
    attr.background_pixel = WhitePixel(display, screen);

    window = XCreateWindow(display, root, x, y, screenWidth, screenHeight,
             border_width, CopyFromParent, InputOutput,
             visual, attribute_mask, &attr);

    XSizeHints size_hints;
	char *window_name;
	XClassHint class_hints;
	XWMHints window_manager_hints;

    size_hints.x = x;
	size_hints.y = y;
	size_hints.width = screenWidth;
	size_hints.height = screenHeight;
	size_hints.min_width = screenWidth;
	size_hints.min_height = screenHeight;
	size_hints.base_width = screenWidth;
	size_hints.base_height = screenHeight;
	size_hints.flags = USPosition | USSize | PMinSize | PBaseSize;
	window_name = argv[0];
	class_hints.res_class = application_class;
	class_hints.res_name = window_name;
	window_manager_hints.flags = InputHint | StateHint;
	window_manager_hints.initial_state = NormalState;
	window_manager_hints.input = True;

	XSetWMNormalHints(display, window, &size_hints);
	XStoreName(display, window, window_name);
	XSetClassHint(display, window, &class_hints);
	XSetWMHints(display, window, &window_manager_hints);

    XMapRaised(display, window);
    XFlush(display);

    while (!done)
    {
        XNextEvent(display, &event);
        if (event.type == Expose)
        {

        }
        else if (event.type == ButtonPress)
        {
            done = 1;
        }
        else if (event.type == KeyPress)
        {
            done = 1;
        }
    }

    return 0;
}
