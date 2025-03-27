#ifndef G_VEC_H
#define G_VEC_H

typedef struct Vector2u {
    uint32 x, y;
} Vector2u;

typedef struct Vector3u {
    uint32 x, y, z;
} Vector3u;

typedef struct Vector3f {
    float32 x, y, z;
} Vector3f;

typedef struct Vec3fArray {
    Vector3f *V;
    uint32 size;
    uint32 capacity;
} Vec3fArray;

void Vector3fArrayAppend(Arena *arena, Vec3fArray *a, Vector3f x);

#define Vector3ArrayDump(A) do{\
    for (int i = 0; i < (A).size; i++){\
        printf("x: %f y: %f z: %f\n", A.V[i].x, A.V[i].y, A.V[i].z);\
    };\
}while(0);


#endif // G_VEC_H
