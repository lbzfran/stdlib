
#include "graphics.h"
#include "graphics_draw_linux.c"


// X11 Context

void *GWindowInit(GCtx *context, GWindowInfo info, uint32 event_masks)
    // char *win_name, uint32 width, uint32 height, uint32 event_masks
{
    GWindow *win = PushStruct(&ctx->arena, GWindow);

    win->alive = true;
    win->event = (XEvent){0};

    win->keyMods = 0;
    win->width = info.width;
    win->height = info.height;

    win->display = XOpenDisplay((char *)NULL);
    win->screen = DefaultScreen(win->display);

    Display *display = win->display;
    int screen = win->screen;


    win->black = BlackPixel(display, screen);
    win->white = WhitePixel(display, screen);

    win->window = XCreateSimpleWindow(display, DefaultRootWindow(display),
            0, 0, win->width, win->height, 2, win->white, win->black);

    Window window = win->window;

    char *actualTitle = info->title;
    if (actualTitle == NULL)
    {
        actualTitle = "generic window";
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


    XSetStandardProperties(display, window, actualName, "", None, NULL, 0, NULL);

    // defaults
    XSelectInput(display, window, event_masks);


    XWindowAttributes attr = {0};
    XGetWindowAttributes(display, window, &attr);

    win->gc = XCreateGC(display, window, 0, NULL);

    win->wm_delete_window = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, window, &win->wm_delete_window, 1);

    GWindowFont(win, "6x13");

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
    win_size_hints->base_width = win->width;
    win_size_hints->base_height = win->height;

    XSetWMNormalHints(display, window, win_size_hints);
    XFree(win_size_hints);

    // extensions
    Bool autorepeat_supported;
    XkbSetDetectableAutoRepeat(display, 1, &autorepeat_supported);
    if (!autorepeat_supported)
    {
        fprintf(stderr, "INFO: Autorepeat not supported.\n");
    }

    win->colormap = DefaultColormap(display, screen);

    XClearWindow(display, window);

    XSetForeground(display, win->gc, win->white);
    XSetBackground(display, win->gc, win->black);

    // NOTE(liam): like XMapWindow but also raises to top of stack.
    XMapRaised(display, window);
    XFlush(display);

    return win;
}

void GWindowFree(void *win)
{
    // NOTE(liam): win should not be referred to after free,
    // unless GWindowInit is called on it again.
    if (win->font)
    {
        XFreeFont(win->display, win->font);
    }
    if (win->colormap)
    {
        XFreeColormap(win->display, win->colormap);
    }
    if (win->gc)
    {
        XFreeGC(win->display, win->gc);
    }
    XDestroyWindow(win->display, win->window);
    XCloseDisplay(win->display);
}


void GWindowClear(void *win)
{
    XClearWindow(win->display, win->window);
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

static GEKeyMod GEGetState(uint32 state)
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

GEvent GWindowEvent(void *win)
{

    GEvent result = GE_Null;
    win->event = (XEvent){0};

    XNextEvent(win->display, &win->event);

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
    /*else if (XEventsQueued(win->display, QueuedAfterReading))*/
    /*{*/
    /*    XPeekEvent(win->display, &next_event);*/
    /*}*/

    switch (win->event.type)
    {
        case ClientMessage:
        {
            if ((Atom)win->event.xclient.data.l[0] == win->wm_delete_window)
            {
                win->alive = false;
                result = GE_Kill;
            }
        } break;
        // NOTE(liam): I won't really care about exposure for any of my apps.
        /*case Expose:*/
        /*{*/
        /*    if (win->event.xexpose.count == 0)*/
        /*    {*/
        /*    }*/
        /*} break;*/
        case ConfigureNotify:
        {
            XConfigureEvent xce = win->event.xconfigure;

            if ((xce.width <= win->width && xce.width >= 400) &&
                    (xce.height <= win->height && xce.height >= 300))
            {
                win->width = xce.width;
                win->height = xce.height;
                /*GWinDraw(win);*/
            }
            result = GE_Notify;
        } break;
        case KeyPress:
        {
            KeySym key = XLookupKeysym(&win->event.xkey, 0);

            // TODO(liam): record key presses.
            GEKeyMod mods = GEGetState(win->event.xkey.state);

            // TODO(liam): apply the mod to key.
            win->keyDown = GEGetKey(key);
            win->keyMods = mods;

            if (win->keyDown)
            {
                printf("key down: '%d' (actual: '%ld') with mod(s) '%d'.\n", win->keyDown, key, win->keyMods);
            }

            result = GE_KeyDown;
        } break;
        case KeyRelease:
        {
            KeySym key = XLookupKeysym(&win->event.xkey, 0);

            GEKeyMod mods = GEGetState(win->event.xkey.state);

            win->keyReleased = GEGetKey(key);
            win->keyMods = mods;

            if (win->keyReleased)
            {
                printf("key released: '%d' (actual: '%ld') with mod(s) '%d'.\n", win->keyReleased, key, win->keyMods);
            }

            result = GE_KeyRelease;
        } break;
        case ButtonPress:
        {
            uint8 mouseKey = win->event.xbutton.button;
	    win->mouseKey = mouseKey;

	    printf("Mouse pressed: '%d' at (%i, %i)\n", win->mouseKey, win->event.xbutton.x, win->event.xbutton.y);


            result = GE_MousePress;
        } break;
        case MotionNotify:
	{
	    win->mouseX = win->event.xmotion.x;
	    win->mouseY = win->event.xmotion.y;

	    result = GE_MouseMove;
	} break;
        default:
        {} break;
    }

    return result;
}
