#ifndef GRAPHICS_DRAW_H
#define GRAPHICS_DRAW_H

#include "base/base.h"
#include "math/math.h"
#include "gp_window.h"

typedef struct Color {
    uint8 r, g, b;
    uint8 a;
} Color;

Color GColor(uint8, uint8, uint8, uint8);
uint32 GColorConvert(Color c);

// NOTE(liam): software rendering
// could be quickly deprecated
void GDrawWrite(GWindow *win, char *s, memory_index len);
void GDrawPixel(GWindow *win, Vector2u pos, uint32 color_pixel);
void GDrawLine(GWindow *win, Vector2u a, Vector2u b, uint32 color_pixel);
void GDrawTriangle(GWindow *win, Vector2u a, Vector2u b, Vector2u c, uint32 color_pixel);

#endif // GRAPHICS_DRAW_H
