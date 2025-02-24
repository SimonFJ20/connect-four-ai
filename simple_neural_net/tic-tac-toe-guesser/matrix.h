#ifndef MATRIX_H
#define MATRIX_H

#include <stddef.h>

typedef struct {
    size_t cols;
    double values[];
} Mx1;

Mx1* mx1_new(size_t cols);
void mx1_free(Mx1* matrix);
Mx1* mx1_from(double* values, size_t cols);
Mx1* mx1_clone(Mx1* matrix);

typedef struct {
    size_t rows;
    size_t cols;
    double values[];
} Mx2;

Mx2* mx2_new(size_t rows, size_t cols);
void mx2_free(Mx2* matrix);
Mx2* mx2_from(double* values, size_t rows, size_t cols);
Mx2* mx2_clone(Mx2* matrix);

#endif
