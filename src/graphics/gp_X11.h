#ifndef GRAPHICS_X11_H
#define GRAPHICS_X11_H

#define CTX_X11 1

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/XKBlib.h>

typedef struct GInner_X11 {
    // might get rid of this tbh
    Display *display;
    int screen;
    Window root, window;
    Atom wm_delete_window;

    XEvent event;

    GC gc;
    XVisualInfo *visual;
    XFontStruct *font;
    uint32 fontHeight;

    Colormap colormap;
    uint32 black, white;

    uint32 event_masks;
} GInner_X11;

#endif
