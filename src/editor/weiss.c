/*
 * Graphical version of editor.
 *
 */

#include "base/base.h"
#include "os/os.h"
//#define RAYLIB_IMPLEMENTATION
#include "raylib.h"

#define WEISS_TAB_AS_SPACES 1
#define WEISS_TAB_STOP 4
#define WEISS_QUIT_CONFIRM_COUNTER 1
#define WEISS_BACKSPACE_APPEND 1
#define WEISS_DISPLAY_DIRT_COUNTER 1
#define WEISS_SCROLL_Y_MARGIN 7

const int screenWidth = 800;
const int screenHeight = 600;

typedef struct erow {
    int idx;
    int size;
    int rsize;
    char *chars;
    char *render;
    unsigned char *hl;
    int hl_open_comment;
} erow;

struct editorConfig {
    int cx, cy; // cursor position
    int rx; // added ry to keep track of last farthest y
    int px;
    int rowOffset; // offset
    int colOffset;
    int screenRows;
    int screenCols;
    int numRows;
    erow *row;
    int dirty;
    int mode;
    char *fileName;
    char statusMsg[80];
    time_t statusMsgTime;
};

struct editorConfig E;

int main(void)
{

    InitWindow(screenWidth, screenHeight, "weiss editor");
    SetTargetFPS(60);
    while (!WindowShouldClose())
    {
        BeginDrawing();
        {
            ClearBackground(DARKGRAY);

            DrawRectangle(200, 300, 100, 200, WHITE);

            DrawText("Congrats!", 190, 200, 20, LIGHTGRAY);
        }
        EndDrawing();
    }
    CloseWindow();
    return 0;
}
