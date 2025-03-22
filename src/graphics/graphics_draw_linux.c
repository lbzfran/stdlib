
#include "graphics_linux.h"

#include "math/matrix.h"

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

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


// BASICS

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

// TODO(liam): big todo
// 3D Rendering

void AssimpLoadMesh(Mesh3D *meshTo, const aiMesh *meshFrom)
{
    // TODO(liam): hehe
}

bool32 AssimpLoadAsset(const char *path)
{
    bool32 result = false;
    const C_STRUCT aiScene *scene = aiImportFile(path, aiProcessPreset_TargetRealTime_MaxQuality);

    if (scene)
    {
        result = true;
    }

    return result;
}

void buildModelMatrix(Arena *arena, Matrix model, Row position, Row orientation, Row scale)
{
    ArenaTemp tmp = ArenaScratchCreate(arena);

    MatrixIdentity(model);

    Matrix matScale     = MatrixIdentity(Matrix4DAlloc(arena));
    Matrix matTranslate = MatrixIdentity(Matrix4DAlloc(arena));
    Matrix matRotYaw    = MatrixIdentity(Matrix4DAlloc(arena));
    Matrix matRotPitch  = MatrixIdentity(Matrix4DAlloc(arena));
    Matrix matRotRoll   = MatrixIdentity(Matrix4DAlloc(arena));

    MatrixAT(matScale, 0, 0) = RowAT(scale, 0);
    MatrixAT(matScale, 1, 1) = RowAT(scale, 1);
    MatrixAT(matScale, 2, 2) = RowAT(scale, 2);

    MatrixAT(matRotYaw, 0, 0) = cosf(RowAT(orientation, 1));
    MatrixAT(matRotYaw, 0, 2) = sinf(RowAT(orientation, 1));
    MatrixAT(matRotYaw, 2, 2) = cosf(RowAT(orientation, 1));
    MatrixAT(matRotYaw, 2, 0) = -1 * sinf(RowAT(orientation, 1));

    MatrixAT(matRotYaw, 1, 1) = cosf(RowAT(orientation, 1));
    MatrixAT(matRotYaw, 1, 2) = -1 * sinf(RowAT(orientation, 1));
    MatrixAT(matRotYaw, 2, 2) = cosf(RowAT(orientation, 1));
    MatrixAT(matRotYaw, 2, 0) = sinf(RowAT(orientation, 1));

    MatrixAT(matRotYaw, 0, 0) = cosf(RowAT(orientation, 1));
    MatrixAT(matRotYaw, 0, 1) = -1 * sinf(RowAT(orientation, 1));
    MatrixAT(matRotYaw, 1, 1) = cosf(RowAT(orientation, 1));
    MatrixAT(matRotYaw, 1, 0) = sinf(RowAT(orientation, 1));

    MatrixAT(matTranslate, 3, 0) = RowAT(position, 0);
    MatrixAT(matTranslate, 3, 1) = RowAT(position, 1);
    MatrixAT(matTranslate, 3, 2) = RowAT(position, 2);

    MatrixDot_(model, matScale, model);
    MatrixDot_(model, matRotYaw, model);
    MatrixDot_(model, matRotPitch, model);
    MatrixDot_(model, matRotRoll, model);
    MatrixDot_(model, matTranslate, model);

    ArenaScratchFree(tmp);
}


