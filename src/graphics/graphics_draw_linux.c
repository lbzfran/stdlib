
#include "graphics_linux.h"

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

void AssimpLoadMesh(Arena *arena, Mesh3D *meshTo, const struct aiMesh *meshFrom)
{
    // TODO(liam): hehe
	for (uint32 i = 0; i < meshFrom->mNumVertices; i++)
	{
		struct aiVector3D meshVertex = meshFrom->mVertices[i];

		/*uint32ArrayAppend(arena, &meshTo->vertices, meshVertex.x);*/
		/*uint32ArrayAppend(arena, &meshTo->vertices, meshVertex.y);*/
		/*uint32ArrayAppend(arena, &meshTo->vertices, meshVertex.z);*/
        ArrayVec3fPush(arena, &meshTo->vertices, (Vector3f){meshVertex.x, meshVertex.y, meshVertex.z});
	}

	for (uint32 i = 0; i < meshFrom->mNumFaces; i++)
	{
		struct aiFace meshFace = meshFrom->mFaces[i];

		ArrayU32Push(arena, &meshTo->faces, meshFace.mIndices[0]);
		ArrayU32Push(arena, &meshTo->faces, meshFace.mIndices[1]);
		ArrayU32Push(arena, &meshTo->faces, meshFace.mIndices[2]);
	}
}

bool32 AssimpLoadAsset(Arena *arena, Mesh3D *meshOut, const char *path)
{
    bool32 result = false;
    const C_STRUCT aiScene *scene = aiImportFile(path, aiProcessPreset_TargetRealtime_MaxQuality);

    if (scene)
    {
        // NOTE(liam): read was successful.
        result = true;

        // NOTE(liam): assume only one mesh for now.
        printf("INFO: number of meshes found: %d\n", scene->mNumMeshes);
        const struct aiMesh *meshIn = scene->mMeshes[0];
        AssimpLoadMesh(arena, meshOut, meshIn);
    }
    else
    {
        printf("ERROR: while loading model '%s'\n", aiGetErrorString());
    }

    aiReleaseImport(scene);
    return result;
}

void buildModelMatrix(Arena *arena, Matrix model, Row position, Row orientation, Row scale)
{
	Assert((model.rows == 4) && (model.cols == 4) && "model matrix must be 4x4");
	Assert((position.cols == 3) && "position must be 1x3");
	Assert((position.cols == orientation.cols) == scale.cols && "orientation/scale must be 1x3");

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

void GDrawMesh(GWin *gw, Matrix model, Matrix view, Matrix projection,
        Mesh3D mesh, uint32 color_pixel)
{
    float mV[16] = {0};
    Matrix mvp = MatrixAlloc(4, 4, mV);

    MatrixDot_(mvp, projection, view);
    MatrixDot_(mvp, mvp, model);

    for (uint32 i = 0; i < mesh.faces.size; i += 3)
    {
        // TODO(liam): should be 3D, but what type?
        Vector3f localA = mesh.vertices.V[mesh.faces.V[i]];
        Vector3f localB = mesh.vertices.V[mesh.faces.V[i + 1]];
        Vector3f localC = mesh.vertices.V[mesh.faces.V[i + 2]];

        // world to clip space: 4d -> 3d
        // i.e., mvp * vec4(a); etc..
        // Matrix worldA = MatrixDotVec4(arena, mvp, vec4(localA));

        // clip to screen
        // i.e, viewport and 3d
        /*Vector2u screenA = ClipToScreen(...);*/

        /*GDrawTriangle(*/
        /*    gw,*/
        /*    (Vector2u){screenA.x, screenA.y},*/
        /*    (Vector2u){screenB.x, screenB.y},*/
        /*    (Vector2u){screenC.x, screenC.y},*/
        /*    color*/
        /*);*/
    }
}
