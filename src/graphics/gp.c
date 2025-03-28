
#include "gp.h"

#include "base/base.h"
#include "os/os.h"

const uint32 screenWidth = 1080;
const uint32 screenHeight = 720;

// used for screen space nav

static GCtx gctx;

GWindowInfo info = {
    (char *)"my win",
    screenWidth,
    screenHeight
};

int main(void)
{
    GWindow *win = (GWindow *)GWindowInit(&gctx, &info, 0);

    while (win->alive)
    {
        GWindowEvent(win);
        GWindowClear(win);

        Color white = GColor(255, 255, 255, 255);

        GDrawTriangle(win, (Vector2u){win->mouseX, win->mouseY}, (Vector2u){300, 200}, (Vector2u){600, 100}, GColorConvert(white));
    }

    GWindowFree(win);
    GCtxFree(&gctx);
    return 0;
}
