#ifndef GRAPHICS_CTX_H
#define GRAPHICS_CTX_H

#include "base/base.h"
#include "os/os.h"
#include "math/math.h"

typedef enum {
    GCtx_Success = 0,
    GCtx_Err, // non-descript terminating error
    GCtx_Err_Memory,
    GCtx_Err_Window,

    GCtx_Wrn, // non-descript non-terminating error
    GCtx_Wrn_Ext,
} GCtxError;

// backing that helps support window creation
// typically OS specific, but since we have OS
// specific things already, the value of this
// decreases
typedef struct GCtx {
    Arena arena;

    // MISC
    uint32 code; // error handle
    uint8 flags[128];
} GCtx;

void GCtxGetError(GCtx *ctx);

void GCtxFree(GCtx *ctx);

#endif // GRAPHICS_CTX_H
