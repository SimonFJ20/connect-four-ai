#include "model.h"
#include "matrix.h"
#include <math.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

static inline double assign_rand_val(double v)
{
    (void)v;
    return rand_val();
}

int model_contruct(Model* model, size_t* layers, size_t layers_size)
{
    size_t* layers_clone = malloc(layers_size * sizeof(size_t));
    memcpy(layers_clone, layers, layers_size * sizeof(size_t));

    size_t weights_size = layers_size - 1;
    Mx2** weights = malloc(weights_size * sizeof(Mx2*));

    size_t biases_size = layers_size - 1;
    Mx1** biases = malloc(biases_size * sizeof(Mx1*));

    for (size_t i = 0; i < layers_size - 1; i++) {
        weights[i] = mx2_new(layers[i], layers[i + 1]);
        mx2_apply(weights[i], assign_rand_val);
        biases[i] = mx1_new(layers[i + 1]);
        mx1_apply(biases[i], assign_rand_val);
    }

    *model = (Model) {
        layers_clone,
        layers_size,
        weights,
        weights_size,
        biases,
        biases_size,
    };
    return 0;
}

void model_destroy(Model* model)
{
    for (size_t i = 0; i < model->layers_size - 1; i++) {
        mx2_free(model->weights[i]);
        mx1_free(model->biases[i]);
    }
    free(model->layers);
    free(model->weights);
    free(model->biases);
}

Mx1* model_feed(Model* model, const Mx1* inputs)
{
    Mx1* outputs = mx1_clone(inputs);

    size_t layers_idcs = model->layers_size - 1;
    for (size_t layer_idx = 0; layer_idx < layers_idcs; ++layer_idx) {
        Mx2* l1 = mx2_clone(model->weights[layer_idx]);
        mx2_transpose(l1);
        mx2_mx1_multiply(l1, outputs);
        mx2_transpose(l1);
        mx2_mx1_add(l1, model->biases[layer_idx]);
        Mx1* l3 = mx2_sum(l1);
        mx1_apply(l3, sigmoid);

        mx2_free(l1);
        mx1_free(outputs);
        outputs = l3;
    }
    return outputs;
}

double rand_val(void)
{
    return (double)rand() / RAND_MAX;
}

double relu(double x)
{
    return x > 0 ? x : 0;
}

double relu_deriv(double x)
{
    return x > 0 ? 1 : 0;
}

double sigmoid(double x)
{
    return 0.5 * (x / (1 + fabs(x)) + 1);
}
double sigmoid_deriv(double x)
{
    return x * (1 - x);
}
