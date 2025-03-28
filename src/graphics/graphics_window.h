#ifndef GRAPHICS_WINDOW_H
#define GRAPHICS_WINDOW_H

#include "base/base.h"
#include "os/os.h"
#include "math/math.h"

typedef enum {
    GE_Null = 0,
    GE_Kill,
    GE_Notify,
    GE_KeyDown,
    GE_KeyRelease,
    GE_MousePress,
    GE_MouseRelease,
    GE_MouseMove,
} GEvent;

typedef enum {
    GE_Mod_Shift     = (1 << 0),
    GE_Mod_Ctrl      = (1 << 1),

    GE_Mod_Alt       = (1 << 2),
    GE_Mod_NumLock   = (1 << 3),
    GE_Mod_Super     = (1 << 5),

    GE_Mod_Mod1      = (1 << 2),
    GE_Mod_Mod2      = (1 << 3),
    GE_Mod_Mod3      = (1 << 4),
    GE_Mod_Mod4      = (1 << 5),
    GE_Mod_Mod5      = (1 << 6),

    GE_Mod_CapsLock  = (1 << 7),
} GEKeyMod;

typedef enum {
    GE_Key_Null = 0,
    GE_Key_Escape = 256,
    GE_Key_Tab,
    GE_Key_Enter,
    GE_Key_Backspace,
    GE_Key_Delete,
    GE_Key_Home,
    GE_Key_End,
    GE_Key_Insert,
    GE_Key_PageUp,
    GE_Key_PageDown,

    GE_Key_Left = 300,
    GE_Key_Right,
    GE_Key_Up,
    GE_Key_Down,
} GEKeyEx;

typedef enum {
    GE_Mouse_Null = 0,
    GE_Mouse_Left,
    GE_Mouse_Middle,
    GE_Mouse_Right,
    GE_Mouse_ScrollUp,
    GE_Mouse_ScrollDown
} GEMouse;


// TODO(liam): get this out of here
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/XKBlib.h>

typedef struct GWin_X11 {
    char *title;
    uint32 width, height;
    uint32 x, y;
    uint32 black, white;

    uint32 keyDown;
    uint32 keyReleased;
    uint32 keyMods;
    GEMouse mouseKey;
    uint32 mouseX, mouseY;

    Display *display;
    int screen;
    Window root, window;
    GC gc;
    XVisualInfo *visual;
    XFontStruct *font;

    XEvent event;

    Colormap colormap;
    Atom wm_delete_window;
    bool32 alive;
} GWindow;

// platform-indepdendent, user provided
typedef struct GWindowInfo {
    char *title;
    uint32 width, height;
} GWindowInfo;

void *GWindowInit(GCtx *context, GWindowInfo *info);
void GWindowFree(void *win);

typedef struct {
    Mesh3D mesh;
    Vector3f position;
    Vector3f scale;
    Vector3f orientation;
} Object3D;

// TODO(liam): abstract away XEvents and make event capture
// easy to use.
GEvent GWindowEvent(void *win);

void GWindowClear(void *win);
#endif
