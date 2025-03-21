
#include "graphics_linux.h"
#include "graphics_draw_linux.c"

#include <X11/XKBlib.h>

void GWinInit(GWin *gw, char *win_name, uint32 width, uint32 height, uint32 event_masks)
{
    gw->alive = true;
    gw->event = (XEvent){0};

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
        StructureNotifyMask | PointerMotionMask | KeyReleaseMask;
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
        fprintf(stderr, "ERROR: XAllocSizeHints unable to allocate memory.\n");
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

    // extensions
    Bool autorepeat_supported;
    XkbSetDetectableAutoRepeat(display, 1, &autorepeat_supported);
    if (!autorepeat_supported)
    {
        fprintf(stderr, "INFO: Autorepeat not supported.\n");
    }

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

static uint32 GEGetKey(KeySym key)
{
    uint32 result = GE_Key_Null;

    switch (key)
    {
	case 65505: // shift
	case 65506:
        case 65507: // ctrl
	case 65508:
        case 65513: // alt
	case 65514:
	{} break;

        case 65293:
	{
	    result = GE_Key_Enter;
	} break;
	case 65289:
	{
	    result = GE_Key_Tab;
	} break;
	case 65307:
	{
	    result = GE_Key_Escape;
	} break;
	case 65288:
	{
	    result = GE_Key_Backspace;
	} break;
	case 65360:
	{
	    result = GE_Key_Home;
	} break;
	case 65367:
	{
	    result = GE_Key_End;
	} break;
	case 65379:
	{
	    result = GE_Key_Insert;
	} break;
	case 65535:
	{
	    result = GE_Key_Delete;
	} break;
	case 65365:
	{
	    result = GE_Key_PageUp;
	} break;
	case 65366:
	{
	    result = GE_Key_PageDown;
	} break;
	case 65361:
	{
	    result = GE_Key_Left;
	} break;
	case 65362:
	{
	    result = GE_Key_Up;
	} break;
	case 65363:
	{
	    result = GE_Key_Right;
	} break;
	case 65364:
	{
	    result = GE_Key_Down;
	} break;
	default:
	{
	    if ((key > 0) && (key < 256))
	    {
		result = key;
	    }
	} break;
    }

    return result;
}

GEKeyMod GEGetState(uint32 state)
{
    GEKeyMod mods = 0;

    if (state & ShiftMask)      mods |= GE_Mod_Shift;
    if (state & ControlMask)    mods |= GE_Mod_Ctrl;
    if (state & Mod1Mask)       mods |= GE_Mod_Mod1;
    if (state & Mod2Mask)       mods |= GE_Mod_Mod2;
    if (state & Mod3Mask)       mods |= GE_Mod_Mod3;
    if (state & Mod4Mask)       mods |= GE_Mod_Mod4;
    if (state & Mod5Mask)       mods |= GE_Mod_Mod5;
    if (state & LockMask)       mods |= GE_Mod_CapsLock;

    return mods;

}

GEvent GWinEvent(GWin *gw)
{

    GEvent result = GE_Null;
    gw->event = (XEvent){0};

    XNextEvent(gw->display, &gw->event);

    // NOTE(liam): handle discarding events.
    // current no use for it but
    // i might find something for it in the future.
    /*XEvent next_event = (XEvent){0};*/
    /*static bool32 discard_flag = false;*/
    /*if (discard_flag)*/
    /*{*/
    /*    discard_flag = false;*/
    /*    return result;*/
    /*}*/
    /*else if (XEventsQueued(gw->display, QueuedAfterReading))*/
    /*{*/
    /*    XPeekEvent(gw->display, &next_event);*/
    /*}*/

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
        // NOTE(liam): I won't really care about exposure for any of my apps.
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
        case KeyPress:
        {
            KeySym key = XLookupKeysym(&gw->event.xkey, 0);

            // TODO(liam): record key presses.
            GEKeyMod mods = GEGetState(gw->event.xkey.state);

            // TODO(liam): apply the mod to key.
            gw->keyDown = GEGetKey(key);
            gw->keyMods = mods;

            if (gw->keyDown)
            {
                printf("key down: '%d' (actual: '%ld') with mod(s) '%d'.\n", gw->keyDown, key, gw->keyMods);
            }

            result = GE_KeyDown;
        } break;
        case KeyRelease:
        {
            KeySym key = XLookupKeysym(&gw->event.xkey, 0);

            GEKeyMod mods = GEGetState(gw->event.xkey.state);

            gw->keyReleased = GEGetKey(key);
            gw->keyMods = mods;

            if (gw->keyReleased)
            {
                printf("key released: '%d' (actual: '%ld') with mod(s) '%d'.\n", gw->keyReleased, key, gw->keyMods);
            }

            result = GE_KeyRelease;
        } break;
        case ButtonPress:
        {
            uint8 mouseKey = gw->event.xbutton.button;
	    gw->mouseKey = mouseKey;

	    printf("Mouse pressed: '%d' at (%i, %i)\n", gw->mouseKey, gw->event.xbutton.x, gw->event.xbutton.y);


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
