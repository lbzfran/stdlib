
#include "math/matrix.h"


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

void GWindowFont(void *win, char *font_name)
{
    if (win->font)
    {
        XFreeFont(win->display, win->font);
    }

    win->font = XLoadQueryFont(win->display, font_name);
    if (win->font == NULL)
    {
        fprintf(stderr, "XLoadQueryFont: failed to load font '%s'\n", font_name);
    }
    XSetFont(win->display, win->gc, win->font->fid);

    win->fontHeight = win->font->ascent + win->font->descent;
}

void GWindowWrite(void *win, char *s, memory_index len)
{
    static int x;
    static int y = 0;
    int string_width;

    string_width = XTextWidth(win->font, s, len);

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
    XDrawString(win->display, win->window, win->gc, x, y, s, len);
}


// BASICS

void GDrawPixel(void *win, Vector2u pos, uint32 color_pixel)
{
    XSetForeground(win->display, win->gc, color_pixel);
    XDrawPoint(win->display, win->window, win->gc, (int)pos.x, (int)pos.y);
}

void GDrawLine(void *win, Vector2u a, Vector2u b, uint32 color_pixel)
{
    XSetForeground(win->display, win->gc, color_pixel);
    XDrawLine(win->display, win->window, win->gc, (int)a.x, (int)a.y, (int)b.x, (int)b.y);
}

void GDrawTriangle(void *win, Vector2u a, Vector2u b, Vector2u c, uint32 color_pixel)
{
    GDrawLine(win, a, b, color_pixel);
    GDrawLine(win, b, c, color_pixel);
    GDrawLine(win, c, a, color_pixel);
}

// TODO(liam): big todo
// 3D Rendering

