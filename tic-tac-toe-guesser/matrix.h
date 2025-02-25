#ifndef MATRIX_H
#define MATRIX_H

#include <stddef.h>

typedef struct {
    size_t cols;
    double values[];
} Mx1;

Mx1* mx1_new(size_t cols);
void mx1_free(Mx1* matrix);
Mx1* mx1_from(const double* values, size_t cols);
Mx1* mx1_clone(const Mx1* matrix);
double* mx1_at(Mx1* matrix, size_t cols);
const double* mx1_at_const(const Mx1* matrix, size_t cols);

typedef struct {
    size_t rows;
    size_t cols;
    double values[];
} Mx2;

void mx2_construct(Mx2* matrix, size_t rows, size_t cols);
Mx2* mx2_new(size_t rows, size_t cols);
void mx2_free(Mx2* matrix);
Mx2* mx2_from(const double* values, size_t rows, size_t cols);
Mx2* mx2_clone(const Mx2* matrix);
double* mx2_at(Mx2* matrix, size_t rows, size_t cols);
const double* mx2_at_const(const Mx2* matrix, size_t rows, size_t cols);

void mx2_transpose(Mx2* matrix);
void mx2_mx1_add(Mx2* lhs, const Mx1* rhs);
void mx2_mx1_multiply(Mx2* lhs, const Mx1* rhs);
Mx1* mx2_sum(const Mx2* matrix);

typedef double (*ApplyFunc)(double value);
void mx1_apply(Mx1* matrix, ApplyFunc func);
void mx2_apply(Mx2* matrix, ApplyFunc func);

void mx1_add(Mx1* lhs, const Mx1* rhs);
void mx1_sub(Mx1* lhs, const Mx1* rhs);
void mx1_multiply(Mx1* lhs, const Mx1* rhs);
double mx1_sum(const Mx1* matrix);

void mx2_add(Mx2* lhs, const Mx2* rhs);
void mx2_multiply(Mx2* lhs, const Mx2* rhs);

void mx1_double_add(Mx1* lhs, double rhs);
void mx1_double_multiply(Mx1* lhs, double rhs);

void mx2_double_add(Mx2* lhs, double rhs);
void mx2_double_multiply(Mx2* lhs, double rhs);

#endif
