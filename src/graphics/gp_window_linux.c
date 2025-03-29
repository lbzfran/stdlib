
#include "gp_ctx.h"
#include "gp_window.h"

#include "gp_X11.h"

// X11 Context
GWindow *GWindowInit(GCtx *ctx, GWindowInfo *info, uint32 event_masks)
    // char *win_name, uint32 width, uint32 height, uint32 event_masks
{
    GWindow *win = PushStruct(&ctx->arena, GWindow);
    GInner_X11 *handler = PushStruct(&ctx->arena, GInner_X11);
    win->handler = (GInner)handler;


    win->alive = true;

    win->keyMods = 0;
    win->width = info->width;
    win->height = info->height;

    handler->event = (XEvent){0};
    handler->display = XOpenDisplay((char *)NULL);
    if (handler->display == NULL)
    {
        ctx->code = GCtx_Err_Window;
    }

    handler->screen = XDefaultScreen(handler->display);

    Display *display = handler->display;
    int screen = handler->screen;


    handler->black = XBlackPixel(display, screen);
    handler->white = XWhitePixel(display, screen);

    handler->window = XCreateSimpleWindow(display, DefaultRootWindow(display),
            0, 0, win->width, win->height, 2, handler->white, handler->black);

    Window window = handler->window;

    char *actualTitle = info->title;
    if (actualTitle == NULL)
    {
        actualTitle = "generic window";
    }
    memory_index size = StringLength((uint8 *)actualTitle);
    win->title = PushArray(&ctx->arena, char, size);
    MemoryCopy(win->title, actualTitle, size);

    if (event_masks == 0)
    {
        event_masks = ButtonPressMask | KeyPressMask |
        StructureNotifyMask | PointerMotionMask | KeyReleaseMask;
    }
    else
    {
	// ensure this is true no matter what to keep track of resizing.
        event_masks |= StructureNotifyMask;
    }
    handler->event_masks = event_masks;

    XSetStandardProperties(display, window, win->title, "", None, NULL, 0, NULL);

    // defaults
    XSelectInput(display, window, event_masks);

    XWindowAttributes attr = {0};
    XGetWindowAttributes(display, window, &attr);

    handler->gc = XCreateGC(display, window, 0, NULL);

    handler->wm_delete_window = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, window, &handler->wm_delete_window, 1);

    GWindowFont(win, "6x13");

    XSizeHints *win_size_hints = XAllocSizeHints();
    if (!win_size_hints)
    {
        fprintf(stderr, "ERROR: XAllocSizeHints unable to allocate memory.\n");
        ctx->code = GCtx_Err_Memory;
        return win;
    }

    // NOTE(liam): preferred window sizing
    win_size_hints->flags = PSize | PMinSize;

    win_size_hints->min_width   = 300;
    win_size_hints->min_height  = 200;
    win_size_hints->base_width  = win->width;
    win_size_hints->base_height = win->height;

    XSetWMNormalHints(display, window, win_size_hints);
    XFree(win_size_hints);

    // extensions
    Bool autorepeat_supported;
    XkbSetDetectableAutoRepeat(display, 1, &autorepeat_supported);
    if (!autorepeat_supported)
    {
        fprintf(stderr, "WARN: Autorepeat not supported.\n");
        ctx->code = GCtx_Wrn_Ext;
    }


    handler->colormap = XDefaultColormap(display, screen);

    // first render
    XClearWindow(display, window);

    XSetForeground(display, handler->gc, handler->white);
    XSetBackground(display, handler->gc, handler->black);

    // NOTE(liam): like XMapWindow but also raises to top of stack.
    XMapRaised(display, window);
    XFlush(display);

    return win;
}

void GWindowFree(GWindow *win)
{
    GInner_X11 *handler = (GInner_X11 *)win->handler;

    // NOTE(liam): win should not be referred to after free,
    // unless GWindowInit is called on it again.
    if (handler->font)
    {
        XFreeFont(handler->display, handler->font);
    }
    if (handler->colormap)
    {
        XFreeColormap(handler->display, handler->colormap);
    }
    if (handler->gc)
    {
        XFreeGC(handler->display, handler->gc);
    }
    XDestroyWindow(handler->display, handler->window);
    XCloseDisplay(handler->display);
}


void GWindowClear(GWindow *win)
{
    GInner_X11 *handler = (GInner_X11 *)win->handler;

    XClearWindow(handler->display, handler->window);
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

uint32 GWindowEvent(GWindow *win)
{
    GInner_X11 *handler = (GInner_X11 *)win->handler;

    uint32 result = GE_Null;
    handler->event = (XEvent){0};

    /*XNextEvent(handler->display, &handler->event);*/

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

    if (XCheckTypedWindowEvent(handler->display, handler->window, ClientMessage, &handler->event))
    {
        if ((Atom)handler->event.xclient.data.l[0] == handler->wm_delete_window)
        {
            win->alive = false;
            result |= GE_Kill;
        }
    }
    else
    {
        while (XCheckWindowEvent(handler->display, handler->window, handler->event_masks, &handler->event))
        {
            switch (handler->event.type)
            {
                // NOTE(liam): I won't really care about exposure for any of my apps.
                /*case Expose:*/
                /*{*/
                /*    if (win->event.xexpose.count == 0)*/
                /*    {*/
                /*    }*/
                /*} break;*/
                case ConfigureNotify:
                    {
                        XConfigureEvent xce = handler->event.xconfigure;

                        if ((xce.width <= win->width && xce.width >= 400) &&
                                (xce.height <= win->height && xce.height >= 300))
                        {
                            win->width = xce.width;
                            win->height = xce.height;
                            /*GWinDraw(win);*/
                        }
                        result |= GE_Notify;
                    } break;
                case KeyPress:
                    {
                        KeySym key = XLookupKeysym(&handler->event.xkey, 0);

                        // TODO(liam): record key presses.
                        GEKeyMod mods = GEGetState(handler->event.xkey.state);

                        // TODO(liam): apply the mod to key.
                        win->keyDown = GEGetKey(key);
                        win->keyMods = mods;

                        if (win->keyDown)
                        {
                            printf("key down: '%d' (actual: '%ld') with mod(s) '%d'.\n", win->keyDown, key, win->keyMods);
                        }

                        result |= GE_KeyDown;
                    } break;
                case KeyRelease:
                    {
                        KeySym key = XLookupKeysym(&handler->event.xkey, 0);

                        GEKeyMod mods = GEGetState(handler->event.xkey.state);

                        win->keyReleased = GEGetKey(key);
                        win->keyMods = mods;

                        if (win->keyReleased)
                        {
                            printf("key released: '%d' (actual: '%ld') with mod(s) '%d'.\n", win->keyReleased, key, win->keyMods);
                        }

                        result |= GE_KeyRelease;
                    } break;
                case ButtonPress:
                    {
                        uint8 mouseKey = handler->event.xbutton.button;
                        win->mouseKey = mouseKey;

                        printf("Mouse pressed: '%d' at (%i, %i)\n", win->mouseKey, handler->event.xbutton.x, handler->event.xbutton.y);


                        result |= GE_MousePress;
                    } break;
                case MotionNotify:
                    {
                        win->mouseX = handler->event.xmotion.x;
                        win->mouseY = handler->event.xmotion.y;

                        result |= GE_MouseMove;
                    } break;
                default:
                    {} break;
            }
        }
    }

    return result;
}

void GWindowFont(GWindow *win, char *font_name)
{
    GInner_X11 *handler = (GInner_X11 *)win->handler;
    if (handler->font)
    {
        XFreeFont(handler->display, handler->font);
    }

    handler->font = XLoadQueryFont(handler->display, font_name);
    if (handler->font == NULL)
    {
        fprintf(stderr, "XLoadQueryFont: failed to load font '%s'\n", font_name);
    }
    XSetFont(handler->display, handler->gc, handler->font->fid);

    handler->fontHeight = handler->font->ascent + handler->font->descent;
}
