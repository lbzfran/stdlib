#ifndef MATRIX_H
#define MATRIX_H

// NOTE(liam): replace stdint include with base
#include <stdio.h>
#include <math.h>

#include "base/base.h"

typedef struct Matrix {
    size_t rows;
    size_t cols;
    float *V;
} Matrix, Row;

#define MatrixAT(m, i, j) ((m).V[(i) * m.cols + (j)])
#define RowAT(r, i) MatrixAT(r, 0, i)

#define MatrixSafeAT(m, i, j) MatrixAT(m, ClampDown(i, m.rows - 1), ClampDown(j, m.cols - 1))
#define RowSafeAT(r, i) MatrixAT(r, 0, ClampDown(i, r.cols - 1))

Matrix MatrixAlloc(size_t, size_t, float*);

#define MatrixArenaAlloc(arena, i, j) (MatrixAlloc((i), (j), PushArray(arena, float, (i) * (j))))
#define Matrix4DAlloc(arena) (MatrixArenaAlloc(arena, 4, 4))
#define RowArenaAlloc(arena, i) MatrixArenaAlloc(arena, 1, i)

void MatrixPrint_(Matrix a, const char*);
#define MatrixPrint(m) MatrixPrint_(m, #m)

void MatrixRandomize(RandomSeries *, Matrix, float, float);
void MatrixFill(Matrix, float);
Matrix MatrixIdentity(Matrix);

Row MatrixRow(Matrix, size_t);

void MatrixCopy_(Matrix, Matrix);
Matrix MatrixCopy(Arena *, Matrix);

void MatrixSliceRow_(Matrix, Matrix, size_t, size_t);
Matrix MatrixSliceRow(Arena *, Matrix, size_t, size_t);

Matrix MatrixReturnM_(Arena *, Matrix, Matrix, void (*)(Matrix, Matrix, Matrix));
void MatrixAddM_(Matrix, Matrix, Matrix);
void MatrixSubM_(Matrix, Matrix, Matrix);
void MatrixMulM_(Matrix, Matrix, Matrix); // Hadamard Product

#define MatrixAddM(arena, a, b) MatrixReturnM_(arena, a, b, MatrixAddM_)
#define MatrixSubM(arena, a, b) MatrixReturnM_(arena, a, b, MatrixSubM_)
#define MatrixMulM(arena, a, b) MatrixReturnM_(arena, a, b, MatrixMulM_)

void MatrixDot_(Matrix, Matrix, Matrix);
Matrix MatrixDot(Arena *arena, Matrix a, Matrix b);

Matrix MatrixReturnS_(Arena *, Matrix, float, void (*)(Matrix, Matrix, float));
void MatrixAddS_(Matrix, Matrix, float);
void MatrixSubS_(Matrix, Matrix, float);
void MatrixMulS_(Matrix, Matrix, float);

#define MatrixAddS(arena, a, f) MatrixReturnS_(arena, a, f, MatrixAddS_)
#define MatrixSubS(arena, a, f) MatrixReturnS_(arena, a, f, MatrixSubS_)
#define MatrixMulS(arena, a, f) MatrixReturnS_(arena, a, f, MatrixMulS_)

void MatrixSum(Matrix, Matrix);
void MatrixTranspose_(Matrix, Matrix);
Matrix MatrixTranspose(Arena *, Matrix);

void MatrixShuffleValue(RandomSeries *, Matrix);

bool32 MatrixRandomShuffleRow(RandomSeries *, Matrix, size_t, size_t *);
bool32 MatrixRandomShuffleCol(RandomSeries *, Matrix, size_t, size_t *);
bool32 MatrixShuffleRow(Matrix, size_t *, size_t);
bool32 MatrixShuffleCol(Matrix, size_t *, size_t);
#define RowRandomShuffle MatrixRandomShuffleCol
#define RowShuffle MatrixShuffleCol

void MatrixApply(Matrix, float (*)(float));

#endif
