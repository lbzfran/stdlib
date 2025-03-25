/*typedef GLXContext (*glXCreateContextAttribsARBProc)(Display*, GLXFBConfig, GLXContext, bool32, const int*);*/
// void GWindowGLInit(GWindow *gw)
// {
// 	XSetWindowAttributes attr = {0,};

//     gw->alive = true;
//     gw->event = (XEvent){0};
//     gw->key = (KeySym){0};
//     gw->width = screenWidth;
//     gw->height = screenHeight;

//     gw->display = XOpenDisplay((char *)NULL);
//     gw->screen = DefaultScreen(gw->display);
//     gw->root = RootWindow(gw->display, gw->screen);

//     int visdata[] = {
//         GLX_RENDER_TYPE, GLX_RGBA_BIT,
//         GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
//         GLX_DOUBLEBUFFER, True,
//         GLX_RED_SIZE, 8,
//         GLX_GREEN_SIZE, 8,
//         GLX_BLUE_SIZE, 8,
//         GLX_ALPHA_SIZE, 8,
//         GLX_DEPTH_SIZE, 16,
//         None
//     };

//     gw->fbconfigs = glXChooseFBConfig(gw->display, gw->screen, visdata, &gw->numfbconfigs);
//     gw->fbconfig = 0;

//     for (uint32 i = 0; i < gw->numfbconfigs; i++)
//     {
//         gw->visual = (XVisualInfo *)glXGetVisualFromFBConfig(gw->display, gw->fbconfigs[i]);

//         if (!visual)
//         {
//             continue;
//         }

//         pict_format = XRenderFindVisualFormat(gw->display, gw->visual->visual);
//         if (!pict_format)
//         {
//             continue;
//         }

//         gw->fbconfig = gw->fbconfigs[i];
//         if (pict_format->direct.alphaMask > 0)
//         {
//             break;
//         }
//         XFree(visual);
//     }

//     if (!fbconfig)
//     {

//     }

//     // describe_fbconfig

//     gw->colormap = XCreateColormap(gw->display, gw->visual->visual, AllocNone);

//     attr.colormap = gw->colormap;
//     attr.background_pixmap = None;
//     atrr.border_pixmap = None;
//     attr.border_pixel = 0;
//     attr.event_mask = ExposureMask | ButtonPressMask | EnterWindowMask |
//                       LeaveWindowMask |KeyPressMask | StructureNotifyMask;

//     int x = gw->width / 2;
//     int y = gw->height / 2;
//     int attr_mask = CWColormap | CWBorderPixel | CWEventMask;

//     gw->window = XCreateWindow(
//         gw->display,
//         gw->root,
//         x, y,
//         gw->width, gw->height,
//         gw->visual->depth,
//         InputOutput,
//         gw->visual->visual,
//         attr_mask,
//         &attr
//     );

//     int glXattr[] = { None };
//     gw->glx_window = glXCreateWindow(gw->display, gw->fbconfig, gw->window, glXattr);

//     XEvent event;

// 	XSizeHints hints;
//     hints.x = x;
//     hints.y = y;
//     hints.width = gw->width;
//     hints.height = gw->height;
//     hints.flags = USPosition | USSize;

// 	XWMHints *startup_state = XAllocWMHints();
//     startup_state->initial_state = NormalState;
//     startup_state->flags = StateHint;

//     XSetWMProperties(gw->display, gw->window, NULL, NULL, NULL, 0,
//                      &hints, startup_state, NULL);

//     XFree(startup_state);

//     XMapRaised(gw->display, gw->window);
//     XIfEvent(gw->display, &gw->event, WaitForMapNotify, (char *)&gw->window);

//     if ((gw->wm_delete_window = XInternAtom(gw->display, "WM_DELETE_WINDOW", 0)) != None) {
// 		XSetWMProtocols(gw->display, gw->window, &gw->wm_delete_window, 1);
// 	}
// }
