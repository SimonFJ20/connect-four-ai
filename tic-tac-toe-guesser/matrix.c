#include "matrix.h"
#include <stddef.h>
#include <stdlib.h>

Mx1* mx1_new(size_t cols)
{
    Mx1* matrix = malloc(sizeof(Mx1) + cols * sizeof(double));
    return matrix;
}

void mx1_free(Mx1* matrix)
{
    free(matrix);
}

Mx1* mx1_from(double* values, size_t cols)
{
    Mx1* matrix = mx1_new(cols);
    for (size_t i = 0; i < cols; ++i) {
        matrix->values[i] = values[i];
    }
    return matrix;
}

Mx1* mx1_clone(Mx1* matrix)
{
    return mx1_from(matrix->values, matrix->cols);
}

Mx2* mx2_new(size_t rows, size_t cols)
{
    Mx2* matrix = malloc(sizeof(Mx1) + rows * cols * sizeof(double));
    return matrix;
}

void mx2_free(Mx2* matrix)
{
    free(matrix);
}

Mx2* mx2_from(double* values, size_t rows, size_t cols)
{
    Mx2* matrix = mx2_new(rows, cols);
    for (size_t i = 0; i < rows * cols; ++i) {
        matrix->values[i] = values[i];
    }
    return matrix;
}

Mx2* mx2_clone(Mx2* matrix)
{
    return mx2_from(matrix->values, matrix->rows, matrix->cols);
}
