#ifndef GRAPHICS_LINUX_H
#define GRAPHICS_LINUX_H

#include "base/base.h"
#include "os/os.h"

#include "vector.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>

typedef enum {
    GE_Null = 0,
    GE_Kill,
    GE_Notify,
    GE_KeyPress,
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

typedef struct Color {
    uint8 r, g, b;
    uint8 a;
} Color;

Color GColor();
uint32 GColorConvert(Color c);

typedef struct GWin {
    uint32 width, height;
    uint32 black, white;

    uint8 flags[256];

    Display *display;
    int screen;
    Window root, window;
    GC gc;
    /*Visual *visual;*/
    XVisualInfo *visual;
    XFontStruct *font;

    XEvent event;
    uint32 keyPressed;
    uint32 keyReleased;
    uint32 keyMods;
    GEMouse mouseKey;
    uint32 mouseX, mouseY;

    Colormap colormap;

    uint32 fontHeight;
    uint32 screenRows, screenCols;

    Atom wm_delete_window;
    bool32 alive;
} GWin;


void GWinInit(GWin *gw, char *win_name, uint32 width, uint32 height, uint32 event_masks);
void GWinFree(GWin *gw);

// TODO(liam): abstract away XEvents and make event capture
// easy to use.
GEvent GWinEvent(GWin *gw);

// future potential:
// GEvent GWinEvent(GWin *gw);

void GWinClear(GWin *gw);

void GWinWrite(GWin *gw, char *s, memory_index len);
void GDrawPixel(GWin *gw, Vector2u pos, uint32 color_pixel);
void GDrawLine(GWin *gw, Vector2u a, Vector2u b, uint32 color_pixel);
void GDrawTriangle(GWin *gw, Vector2u a, Vector2u b, Vector2u c, uint32 color_pixel);

#endif // GRAPHICS_LINUX_H
