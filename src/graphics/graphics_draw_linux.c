#include "graphics_linux.h"

Color GColor()
{
    // returns white.
    return (Color){ 255, 0, 255, 255 };
}

uint32 GColorConvert(Color c)
{
    uint32 color = (c.r << 16) | (c.g << 8) | c.b;

    return color;
}

void GWinFont(GWin *gw, char *font_name)
{
    if (gw->font)
    {
        XFreeFont(gw->display, gw->font);
    }

    gw->font = XLoadQueryFont(gw->display, font_name);
    if (gw->font == NULL)
    {
        fprintf(stderr, "XLoadQueryFont: failed to load font '%s'\n", font_name);
    }
    XSetFont(gw->display, gw->gc, gw->font->fid);

    gw->fontHeight = gw->font->ascent + gw->font->descent;
}

void GWinWrite(GWin *gw, char *s, memory_index len)
{
    static int x;
    static int y = 0;
    int string_width;

    string_width = XTextWidth(gw->font, s, len);

    x = string_width;

    if (x == gw->width)
    {
        x = 0;
    }
    x += 10;
    if (y == gw->height)
    {
        y = string_width;
    }
    y += 10;
    XDrawString(gw->display, gw->window, gw->gc, x, y, s, len);
}

void GDrawPixel(GWin *gw, Vector2u pos, uint32 color_pixel)
{
    XSetForeground(gw->display, gw->gc, color_pixel);
    XDrawPoint(gw->display, gw->window, gw->gc, (int)pos.x, (int)pos.y);
}

void GDrawLine(GWin *gw, Vector2u a, Vector2u b, uint32 color_pixel)
{
    XSetForeground(gw->display, gw->gc, color_pixel);
    XDrawLine(gw->display, gw->window, gw->gc, (int)a.x, (int)a.y, (int)b.x, (int)b.y);
}

void GDrawTriangle(GWin *gw, Vector2u a, Vector2u b, Vector2u c, uint32 color_pixel)
{
    GDrawLine(gw, a, b, color_pixel);
    GDrawLine(gw, b, c, color_pixel);
    GDrawLine(gw, c, a, color_pixel);
}
