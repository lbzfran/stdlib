#ifndef GRAPHICS_WINDOW_H
#define GRAPHICS_WINDOW_H

#include "base/base.h"
#include "os/os.h"
#include "math/math.h"
#include "gp_ctx.h"

/* ----------------------- */
/* INPUT */
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

// the nice thing is, between Null and Escape,
// all the possible keys can be read as char.
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
/* ----------------------- */

typedef enum {
    GE_Null         = (1 << 0),
    GE_Kill         = (1 << 1),
    GE_Notify       = (1 << 2),
    GE_KeyDown      = (1 << 3),
    GE_KeyRelease   = (1 << 4),
    GE_MousePress   = (1 << 5),
    GE_MouseRelease = (1 << 6),
    GE_MouseMove    = (1 << 7),
} GEvent;

// NOTE(liam): window creation should be separate.
// TODO(liam): get this out of here

typedef void* GInner;
typedef struct GWindow {
    bool32 alive;

    char *title;
    uint32 width, height;
    uint32 x, y;

    uint32 keyDown;
    uint32 keyReleased;
    uint32 keyMods;
    GEMouse mouseKey;
    uint32 mouseX, mouseY;

    // OS SPECIFIC window handle
    GInner handler;
} GWindow;

// platform-indepdendent, user provided
typedef struct GWindowInfo {
    char *title;
    uint32 width, height;
} GWindowInfo;

GWindow *GWindowInit(GCtx *context, GWindowInfo *info, uint32 event_masks);
void GWindowFree(GWindow *win);

// TODO(liam): abstract away XEvents and make event capture
// easy to use.
uint32 GWindowEvent(GWindow *win);

// NOTE(liam): idk if this should be here tbh
void GWindowFont(GWindow *win, char *font_name);

void GWindowClear(GWindow *win);
#endif
