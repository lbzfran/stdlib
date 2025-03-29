
#include "gp_draw.h"
#include "math/math.h"

#include "gp_X11.h"

Color GColor(uint8 r, uint8 g, uint8 b, uint8 a)
{
    // returns white.
    return (Color){ r, g, b, a };
}

uint32 GColorConvert(Color c)
{
    uint32 color = (c.r << 16) | (c.g << 8) | c.b;

    return color;
}


// NOTE(liam): added better support for placement, should not be hard.
void GWindowWrite(GWindow *win, char *s, memory_index len)
{
    GInner_X11 *handler = (GInner_X11 *)win->handler;

    static int x;
    static int y = 0;
    int string_width;

    string_width = XTextWidth(handler->font, s, len);

    x = string_width;

    if (x == win->width)
    {
        x = 0;
    }
    x += 10;
    if (y == win->height)
    {
        y = string_width;
    }
    y += 10;
    XDrawString(handler->display, handler->window, handler->gc, x, y, s, len);
}


// BASICS
void GDrawPixel(GWindow *win, Vector2u pos, uint32 color_pixel)
{
    GInner_X11 *handler = (GInner_X11 *)win->handler;

    XSetForeground(handler->display, handler->gc, color_pixel);
    XDrawPoint(handler->display, handler->window, handler->gc, (int)pos.x, (int)pos.y);
}

void GDrawLine(GWindow *win, Vector2u a, Vector2u b, uint32 color_pixel)
{
    GInner_X11 *handler = (GInner_X11 *)win->handler;

    XSetForeground(handler->display, handler->gc, color_pixel);
    XDrawLine(handler->display, handler->window, handler->gc, (int)a.x, (int)a.y, (int)b.x, (int)b.y);
}

void GDrawTriangle(GWindow *win, Vector2u a, Vector2u b, Vector2u c, uint32 color_pixel)
{
    GDrawLine(win, a, b, color_pixel);
    GDrawLine(win, b, c, color_pixel);
    GDrawLine(win, c, a, color_pixel);
}

