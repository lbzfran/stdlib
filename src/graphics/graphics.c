
#include "graphics.h"

void GCtxFree(GCtx *context)
{
    ArenaFree(&context->arena);
}
