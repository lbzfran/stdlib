#ifndef GRAPHICS_GL_H
#define GRAPHICS_GL_H

#include "base/base.h"
#include "os/os.h"

#include "math/math.h"

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

typedef struct {
    ArrayVec3f vertices;
    ArrayU32 faces;
} Mesh3D;

bool32 AssimpLoadAsset(Arena *arena, Mesh3D *meshOut, const char *path);

// DRAWING UTILITIES
void GDrawMesh(GWin *gw, Matrix model, Matrix view, Matrix projection, Mesh3D mesh, uint32 color_pixel);

#endif // GRAPHICS_GL_H
