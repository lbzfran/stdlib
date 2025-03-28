#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "base/base.h"
#include "os/os.h"
#include "math/math.h"

typedef enum {
    GCtx_Success = 0,
    GCtx_Fail, // non-descript terminating error
} GCtxError;


typedef struct Color {
    uint8 r, g, b;
    uint8 a;
} Color;

Color GColor(uint8, uint8, uint8, uint8);
uint32 GColorConvert(Color c);

// backing that helps support window creation
// typically OS specific, but since we have OS
// specific things already, the value of this
// decreases
typedef struct GCtx {
    Arena arena;

    // MISC
    uint32 code;
    uint8 flags[128];
} GCtx;

// initializing this means we want to open a window.
// this will carry its own arena and error handler.
void GCtxFree(GCtx *context);

#endif
