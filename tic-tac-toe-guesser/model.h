#ifndef MODEL_H
#define MODEL_H

#include "matrix.h"
#include <stddef.h>

typedef struct {
    size_t* layers;
    size_t layers_size;

    Mx2** weights;
    size_t weights_size;

    Mx1** biases;
    size_t biases_size;
} Model;

int model_contruct(Model* model, size_t* layers, size_t layers_size);
void model_destroy(Model* model);
Mx1* model_feed(Model* model, const Mx1* inputs);

double rand_val(void);

double relu(double x);
double relu_deriv(double x);

double sigmoid(double x);
double sigmoid_deriv(double x);

#endif
