#ifndef GRAPHICS_DRAW_H
#define GRAPHICS_DRAW_H

#include "base/base.h"
#include "math/math.h"

// NOTE(liam): software rendering
void GDrawWrite(void *win, char *s, memory_index len);
void GDrawPixel(void *win, Vector2u pos, uint32 color_pixel);
void GDrawLine(void *win, Vector2u a, Vector2u b, uint32 color_pixel);
void GDrawTriangle(void *win, Vector2u a, Vector2u b, Vector2u c, uint32 color_pixel);

#endif // GRAPHICS_DRAW_H
