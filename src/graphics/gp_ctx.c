
#include "gp_ctx.h"

void GCtxFree(GCtx *context)
{
    ArenaFree(&context->arena);
}
