
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
    RandomSeries series = RandomSeed(1234);

    GWindow *win = GWindowInit(&gctx, &info, 0);
    Vector2u triPoints[3] = {
        {100, 300}, {300, 200}, {600, 100}
    };
    uint8 colors[3] = {
        200, 200, 200
    };
    uint32 pointValues[12] = {
        100, 150, 200, 250, 300, 450, 500, 550, 600,
        650, 700, 750,
    };
    uint8 capturePoint = 0;

    while (win->alive)
    {
        uint32 ev = GWindowEvent(win);

        if (ev & GE_MousePress)
        {
            capturePoint++;
            if (capturePoint % 3 == 0)
            {
                capturePoint = 0;
            }
        }
        if (ev & GE_MouseMove)
        {
            triPoints[capturePoint % 3] = (Vector2u){win->mouseX, win->mouseY};

            /*capturePoint++;*/
            /*if (capturePoint % 3 == 0)*/
            /*{*/
            /*    capturePoint = 0;*/
            /*}*/
            /*triPoints[capturePoint % 3] = (Vector2u){ pointValues[RandomNextInt(&series) % 12], pointValues[RandomNextInt(&series) % 12]};*/

        }
        /*colors[0]++;*/
        /*if (colors[0] > 250)*/
        /*{*/
        /*    colors[0] = 0;*/
        /*}*/
        /*colors[1] += 2;*/
        /*if (colors[1] > 250)*/
        /*{*/
        /*    colors[1] = 0;*/
        /*}*/
        /*colors[2] += 3;*/
        /*if (colors[2] > 250)*/
        /*{*/
        /*    colors[2] = 0;*/
        /*}*/

        GWindowClear(win);

        Color rainbow = GColor(colors[0], colors[1], colors[2], 255);

        GDrawTriangle(win, triPoints[0], triPoints[1], triPoints[2], GColorConvert(rainbow));
    }

    GWindowFree(win);
    GCtxFree(&gctx);
    return 0;
}
