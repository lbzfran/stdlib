
#include "graphics_linux.h"
#include "graphics_draw_linux.c"

void GWinInit(GWin *gw, char *win_name, uint32 width, uint32 height, uint32 event_masks)
{
    gw->alive = true;
    gw->event = (XEvent){0};
    gw->keyPressed = 0;
    gw->keyReleased = 0;
    gw->keyMods = 0;
    gw->width = width;
    gw->height = height;

    gw->display = XOpenDisplay((char *)NULL);
    gw->screen = DefaultScreen(gw->display);

    Display *display = gw->display;
    int screen = gw->screen;


    gw->black = BlackPixel(display, screen);
    gw->white = WhitePixel(display, screen);

    gw->window = XCreateSimpleWindow(display, DefaultRootWindow(display),
            0, 0, gw->width, gw->height, 2, gw->white, gw->black);

    Window window = gw->window;

    char *actualName = win_name;
    if (actualName == NULL)
    {
	actualName = "generic window";
    }

    if (event_masks == 0)
    {
	event_masks = ButtonPressMask | KeyPressMask |
	StructureNotifyMask | PointerMotionMask;
    }
    else
    {
	// ensure this is on to keep track of resizing.
	event_masks |= StructureNotifyMask;
    }


    XSetStandardProperties(display, window, actualName, "hi", None, NULL, 0, NULL);


    // defaults
    XSelectInput(display, window, event_masks);


    XWindowAttributes attr = {0};
    XGetWindowAttributes(display, window, &attr);

    gw->gc = XCreateGC(display, window, 0, NULL);

    gw->wm_delete_window = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, window, &gw->wm_delete_window, 1);

    GWinFont(gw, "6x13");

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
    win_size_hints->base_width = gw->width;
    win_size_hints->base_height = gw->height;

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

void GWinFree(GWin *gw)
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


void GWinClear(GWin *gw)
{
    XClearWindow(gw->display, gw->window);
}

GEKeyMod GEGetState(uint32 state)
{
    GEKeyMod mods = 0;

    if (state & ShiftMask)
        mods |= GE_Mod_Shift;
    if (state & ControlMask)
        mods |= GE_Mod_Ctrl;
    if (state & Mod1Mask)
        mods |= GE_Mod_Alt;
    if (state & Mod4Mask)
        mods |= GE_Mod_Super;
    if (state & LockMask)
        mods |= GE_Mod_CapsLock;
    if (state & Mod2Mask)
        mods |= GE_Mod_NumLock;

    return mods;

}

GEvent GWinEvent(GWin *gw)
{
    gw->event = (XEvent){0};
    XNextEvent(gw->display, &gw->event);

    GEvent result = GE_Null;
    switch (gw->event.type)
    {
        case ClientMessage:
        {
            if ((Atom)gw->event.xclient.data.l[0] == gw->wm_delete_window)
            {
                gw->alive = false;
		result = GE_Kill;
            }
        } break;
        /*case Expose:*/
        /*{*/
        /*    if (gw->event.xexpose.count == 0)*/
        /*    {*/
        /*    }*/
        /*} break;*/
        case ConfigureNotify:
        {
            XConfigureEvent xce = gw->event.xconfigure;

            if ((xce.width <= gw->width && xce.width >= 400) &&
                    (xce.height <= gw->height && xce.height >= 300))
            {
                gw->width = xce.width;
                gw->height = xce.height;
                /*GWinDraw(gw);*/
            }
	    result = GE_Notify;
        } break;
        case KeyRelease:
	{
	    char key = XLookupKeysym(&gw->event.xkey, 0);
	    gw->keyReleased = key;
	    result = GE_KeyRelease;
	}
        case KeyPress:
        {
	    // TODO(liam): record key presses.
	    //             support for multi-key presses.

	    char key = XLookupKeysym(&gw->event.xkey, 0);
	    uint32 keyMods = GEGetState(gw->event.xkey.state);
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

	    gw->keyPressed = key;
	    gw->keyMods = keyMods;
	    result = GE_KeyPress;
        } break;
        case ButtonPress:
        {
	    uint8 mouseKey = gw->event.xbutton.button;
	    printf("Mouse pressed at (%i, %i)\n", gw->event.xbutton.x, gw->event.xbutton.y);

	    gw->mouseKey = mouseKey;
	    result = GE_MousePress;
        } break;
        case MotionNotify:
	{
	    gw->mouseX = gw->event.xmotion.x;
	    gw->mouseY = gw->event.xmotion.y;

	    result = GE_MouseMove;
	} break;
        default:
        {} break;
    }

    return result;
}
