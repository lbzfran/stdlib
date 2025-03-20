
#include "graphics.h"

#include "base/base.h"
#include "os/os.h"

const uint32 screenWidth = 1080;
const uint32 screenHeight = 720;

// used for screen space nav

int main(void)
{
    GWin gw = {0};

    GWinInit(&gw, "hehe", screenWidth, screenHeight, 0);

    while (gw.alive)
    {
        GWinEvent(&gw);
        GWinClear(&gw);

        Color white = GColor();

        GDrawTriangle(&gw, (Vector2u){gw.mouseX, gw.mouseY}, (Vector2u){300, 200}, (Vector2u){600, 100}, GColorConvert(white));
    }

    GWinFree(&gw);
    return 0;
}
