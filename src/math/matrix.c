#include "matrix.h"

Matrix
MatrixAlloc(size_t rows, size_t cols, float* V)
{
    Matrix res;

    res.rows = rows;
    res.cols = cols;
    res.V = V;

    return res;
}

void
MatrixPrint_(Matrix a, const char* name)
{
    printf("%s = [\n", name);
    for (size_t i = 0; i < a.rows; i++)
    {
        for (size_t j = 0; j < a.cols; j++)
        {
            printf("\t%f ", MatrixAT(a, i, j));
        }
        printf("\n");
    }
    printf("]\n");
}

Matrix
MatrixRow(Matrix a, size_t row)
{
    return (Row) {
        .rows = 1,
        .cols = a.cols,
        .V = &MatrixAT(a, row, 0),
    };
}

void
MatrixRandomize(RandomSeries* series, Matrix a, float low, float high)
{
    for (size_t i = 0; i < a.rows; i++) {
        for (size_t j = 0; j < a.cols; j++) {
            MatrixAT(a, i, j) = RandomBetween(series, low, high);
        }
    }
}

void
MatrixCopy_(Matrix b, Matrix a)
{
    Assert(a.rows == b.rows);
    Assert(a.cols == b.cols);
    for (size_t i = 0; i < b.rows; i++) {
        for (size_t j = 0; j < b.cols; j++) {
            MatrixAT(b, i, j) = MatrixAT(a, i, j);
        }
    }

}

Matrix
MatrixCopy(Arena *arena, Matrix a)
{
    Matrix result = MatrixArenaAlloc(arena, a.rows, a.cols);

    MatrixCopy_(result, a);

    return result;
}

Matrix
MatrixReturnM_(Arena *arena, Matrix a, Matrix b, void (* mfn)(Matrix, Matrix, Matrix))
{
    // NOTE(liam): generic wrapper to return a preallocated matrix,
    // assuming the given manipulation function takes two matrices that
    // are the same shape.
    Matrix result = MatrixArenaAlloc(arena, a.rows, a.cols);

    mfn(result, a, b);

    return result;
}

Matrix
MatrixReturnS_(Arena *arena, Matrix a, float f, void (* mfn)(Matrix, Matrix, float))
{
    Matrix result = MatrixArenaAlloc(arena, a.rows, a.cols);

    mfn(result, a, f);

    return result;
}

void
MatrixFill(Matrix a, float x)
{
    for (size_t i = 0; i < a.rows; i++) {
        for (size_t j = 0; j < a.cols; j++) {
            MatrixAT(a, i, j) = x;
        }
    }
}

Matrix
MatrixIdentity(Matrix a)
{
    for (size_t i = 0; i < a.rows; i++) {
        for (size_t j = 0; j < a.cols; j++) {
            if (i == j)
            {
                MatrixAT(a, i, j) = 1;
            }
            else
            {
                MatrixAT(a, i, j) = 0;
            }
        }
    }

    return a;
}

void
MatrixAddS_(Matrix b, Matrix a, float x)
{
    Assert(a.rows == b.rows);
    Assert(a.cols == b.cols);

    for (size_t i = 0; i < a.rows; i++) {
        for (size_t j = 0; j < a.cols; j++) {
            /*b->V[i][j] = a->V[i][j] + x;*/
            MatrixAT(b, i, j) = MatrixAT(a, i, j) + x;
        }
    }
}

void
MatrixAddM_(Matrix c, Matrix a, Matrix b)
{
    Assert(a.rows == b.rows);
    Assert(a.cols == b.cols);

    for (size_t i = 0; i < a.rows; i++) {
        for (size_t j = 0; j < b.cols; j++) {
            /*c->V[i][j] = a->V[i][j] + b->V[i][j];*/
            /*c.V[i * c.cols + j] = a.V[i * a.cols + j] + b.V[i * b.cols
             * + j];*/
            MatrixAT(c, i, j) =
                MatrixAT(a, i, j) + MatrixAT(b, i, j);
        }
    }
}

void
MatrixSum(Matrix b, Matrix a)
{
    Assert(b.rows == a.rows);
    Assert(b.cols == a.cols);
    for (size_t i = 0; i < a.rows; i++) {
        for (size_t j = 0; j < b.cols; j++) {
            MatrixAT(b, i, j) += MatrixAT(a, i, j);
        }
    }
}

void
MatrixSubS_(Matrix b, Matrix a, float x)
{
    Assert(a.rows == b.rows);
    Assert(a.cols == b.cols);
    for (size_t i = 0; i < a.rows; i++) {
        for (size_t j = 0; j < a.cols; j++) {
            /*b->V[i][j] = a->V[i][j] + x;*/
            MatrixAT(b, i, j) = MatrixAT(a, i, j) - x;
        }
    }
}

void
MatrixSubM_(Matrix c, Matrix a, Matrix b)
{
    Assert(a.rows == b.rows);
    Assert(a.cols == b.cols);
    Assert(a.rows == c.rows);
    for (size_t i = 0; i < a.rows; i++) {
        for (size_t j = 0; j < b.cols; j++) {
            /*c->V[i][j] = a->V[i][j] - b->V[i][j];*/
            MatrixAT(c, i, j) = MatrixAT(a, i, j) - MatrixAT(b, i, j);
        }
    }
}

void
MatrixMulS_(Matrix b, Matrix a, float x)
{
    Assert(a.rows == b.rows);
    Assert(a.cols == b.cols);
    for (size_t i = 0; i < a.rows; i++) {
        for (size_t j = 0; j < a.cols; j++) {
            MatrixAT(b, i, j) = MatrixAT(a, i, j) * x;
        }
    }
}

void
MatrixDivS_(Matrix b, Matrix a, float x)
{
    Assert(a.rows == b.rows);
    Assert(a.cols == b.cols);
    for (size_t i = 0; i < a.rows; i++) {
        for (size_t j = 0; j < a.cols; j++) {
            MatrixAT(b, i, j) = MatrixAT(a, i, j) / x;
        }
    }
}

void
MatrixDot_(Matrix c, Matrix a, Matrix b)
{
    // NOTE(liam): implied that both matrix dimensions are the same.
    Assert(a.cols == b.rows);
    Assert(a.rows == c.rows);
    Assert(c.cols == b.cols);

    for (size_t i = 0; i < a.rows; i++) {
        for (size_t j = 0; j < b.cols; j++) {
            MatrixAT(c, i, j) = 0;
            for (size_t k = 0; k < b.rows; k++) {
                MatrixAT(c, i, j) += MatrixAT(a, i, k) * MatrixAT(b, k, j);
            }
        }
    }
}

Matrix
MatrixDot(Arena *arena, Matrix a, Matrix b)
{
    Matrix result = MatrixArenaAlloc(arena, a.rows, b.cols);

    MatrixDot_(result, a, b);

    return result;
}

void
MatrixMulM_(Matrix c, Matrix a, Matrix b)
{
    Assert(a.rows == b.rows);
    Assert(c.rows == b.rows);
    Assert(c.cols == a.cols);
    Assert(b.cols == a.cols);

    for (size_t i = 0; i < a.rows; i++) {
        for (size_t j = 0; j < b.cols; j++) {
            MatrixAT(c, i, j) = MatrixAT(a, i, j) * MatrixAT(b, i, j);
        }
    }
}

void
MatrixApply(Matrix a, float (*fun)(float))
{
    for (size_t i = 0; i < a.rows; i++) {
        for (size_t j = 0; j < a.cols; j++) {
            a.V[i * a.cols + j] = (*fun)(a.V[i * a.cols + j]);
        }
    }
}

void
MatrixTranspose_(Matrix b, Matrix a)
{
    for (size_t i = 0; i < b.rows; i++) {
        for (size_t j = 0; j < b.cols; j++) {
            /*b.V[i * b.cols + j] = a.V[j * a.cols + i];*/
            MatrixAT(b, i, j) = MatrixAT(a, j, i);
        }
    }
}

Matrix
MatrixTranspose(Arena *arena, Matrix a)
{
    Matrix result = MatrixArenaAlloc(arena, a.cols, a.rows);
    MatrixTranspose_(result, a);

    return result;
}

void
MatrixShuffleValue(RandomSeries *series, Matrix a)
{
    for (size_t i = 0; i < a.rows; i++) {
        for (size_t j = 0; j < a.cols; j++) {
            uint32 row = RandomChoice(series, a.rows);
            uint32 col = RandomChoice(series, a.cols);

            float32 tmp = MatrixAT(a, i, j);
            MatrixAT(a, i, j) = MatrixAT(a, row, col);
            MatrixAT(a, row, col) = tmp;
        }
    }
}

bool32
MatrixRandomShuffleRow(RandomSeries *series, Matrix a, size_t count_per_pair, size_t *shuffle_indices)
{
    // NOTE(liam) shuffle_indices' size must be at least a.cols
    bool32 res = true;
    if (series == NULL)
    {
        res = false;
    }
    else
    {
        size_t *pos = shuffle_indices;

        while (count_per_pair--)
        {
            size_t rowA = RandomChoice(series, a.rows);
            size_t rowB = RandomChoice(series, a.rows);

            if (rowA == rowB) continue;

            if (pos)
            {
                *(pos++) = rowA;
                *(pos++) = rowB;
            }

            for (size_t j = 0; j < a.cols; j++)
            {
                float32 tmp = MatrixAT(a, rowA, j);
                MatrixAT(a, rowA, j) = MatrixAT(a, rowB, j);
                MatrixAT(a, rowB, j) = tmp;
            }
        }
    }
    return res;
}

bool32
MatrixRandomShuffleCol(RandomSeries * series, Matrix a, size_t count_per_pair, size_t *shuffle_indices)
{
    // NOTE(liam): shuffle_indices can be nullable if user does not need to
    // know the shuffle indices. indices are inserted 2 per count_per_pair: (from, to)
    // therefore assume the following: shuffle_indices[count_per_pair * 2]
    bool32 res = true;
    if (series == NULL)
    {
        res = false;
    }
    else
    {
        size_t *pos = shuffle_indices;

        while (count_per_pair--)
        {
            size_t colA = RandomChoice(series, a.cols);
            size_t colB = RandomChoice(series, a.cols);

            if (colA == colB) continue;

            if (pos)
            {
                *(pos++) = colA;
                *(pos++) = colB;
            }

            for (size_t i = 0; i < a.rows; i++)
            {
                float32 tmp = MatrixAT(a, i, colA);
                MatrixAT(a, i, colA) = MatrixAT(a, i, colB);
                MatrixAT(a, i, colB) = tmp;
            }
        }
    }
    return res;
}

bool32
MatrixShuffleRow(Matrix a, size_t *shuffle_indices, size_t count_per_pair)
{
    // NOTE(liam): count_per_pair == sizeof(shuffle_indices) / 2
    bool32 res = true;
    if (shuffle_indices == NULL)
    {
        res = false;
    }
    else
    {
        size_t pos = 0;
        while (count_per_pair-- > 0)
        {
            // TODO(liam): i feel like this operation is not efficiently indexed.
            size_t rowA = shuffle_indices[pos++];
            size_t rowB = shuffle_indices[pos++];

            Assert(rowA < a.rows);
            Assert(rowB < a.rows);
            if (rowA == rowB) continue;

            for (size_t j = 0; j < a.cols; j++)
            {
                float32 tmp = MatrixAT(a, rowA, j);
                MatrixAT(a, rowA, j) = MatrixAT(a, rowB, j);
                MatrixAT(a, rowB, j) = tmp;
            }
        }
    }
    return res;
}

bool32
MatrixShuffleCol(Matrix a, size_t *shuffle_indices, size_t count_per_pair)
{
    bool32 res = true;
    if (shuffle_indices == NULL)
    {
        res = false;
    }
    else
    {
        size_t pos = 0;
        while (count_per_pair-- > 0)
        {
            size_t colA = shuffle_indices[pos++];
            size_t colB = shuffle_indices[pos++];
            /*printf("DEBUG: shuffling col %lu -> %lu.\n", colA, colB);*/

            Assert(colA < a.cols);
            Assert(colB < a.cols);
            if (colA == colB) continue;

            for (size_t i = 0; i < a.rows; i++)
            {
                float32 tmp = MatrixAT(a, i, colA);
                MatrixAT(a, i, colA) = MatrixAT(a, i, colB);
                MatrixAT(a, i, colB) = tmp;
            }
        }
    }
    return res;
}

void
MatrixSliceRow_(Matrix b, Matrix a, size_t start, size_t end)
{
    Assert(b.rows == (end - start));
    Assert(b.cols == a.cols);
    Assert(start != end);

    for (size_t i = start; i < end; i++) {
        for (size_t j = 0; j < a.cols; j++) {
            MatrixAT(b, i - start, j) = MatrixAT(a, i, j);
        }
    }
}

Matrix
MatrixSliceRow(Arena *arena, Matrix a, size_t start, size_t end)
{
    Matrix result = MatrixArenaAlloc(arena, end - start, a.cols);

    MatrixSliceRow_(result, a, start, end);

    return result;
}
