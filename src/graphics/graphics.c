
#include "graphics.h"

#include "base/base.h"
#include "os/os.h"

const uint32 screenWidth = 1080;
const uint32 screenHeight = 720;

// used for screen space nav

int main(void)
{
    Arena arena = {0};
    GWin gw = {0};

    Mesh3D mesh = {0};

    bool32 res = AssimpLoadAsset(&arena, &mesh, "./models/bunny.obj");
    if (!res)
    {
        return res;
    }

    GWinInit(&gw, NULL, screenWidth, screenHeight, 0);

    while (gw.alive)
    {
        GWinEvent(&gw);
        GWinClear(&gw);

        Color white = GColor(255, 255, 255, 255);

        GDrawTriangle(&gw, (Vector2u){gw.mouseX, gw.mouseY}, (Vector2u){300, 200}, (Vector2u){600, 100}, GColorConvert(white));
    }

    GWinFree(&gw);
    return 0;
}
