#include "model.h"
#include "matrix.h"
#include "util.h"
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static inline double randd_dec_f(double in)
{
    (void)in;
    return randd_dec();
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
        mx2_apply(weights[i], randd_dec_f);
        biases[i] = mx1_new(layers[i + 1]);
        mx1_apply(biases[i], randd_dec_f);
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

int model_clone(Model* clone, const Model* m)
{

    size_t* layers_clone = malloc(m->layers_size * sizeof(size_t));
    memcpy(layers_clone, m->layers, m->layers_size * sizeof(size_t));

    Mx2** weights = malloc(m->weights_size * sizeof(Mx2*));
    Mx1** biases = malloc(m->biases_size * sizeof(Mx1*));

    for (size_t i = 0; i < m->weights_size; i++) {
        weights[i] = mx2_clone(m->weights[i]);
    }

    for (size_t i = 0; i < m->biases_size; i++) {
        biases[i] = mx1_clone(m->biases[i]);
    }

    *clone = (Model) {
        layers_clone,
        m->layers_size,
        weights,
        m->weights_size,
        biases,
        m->biases_size,
    };
    return 0;
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

static inline double mutation_dec(double in)
{
    (void)in;
    double r = randd_dec();
    if (r > 0.5) {
        double v = pow(r * 2.0, 2.0) / 4.0;
        return v > 0.5 ? v : 0.0;
    } else {
        double v = pow((1.0 - r) * 2.0, 2.0) / 4.0;
        return v > 0.5 ? -v : 0.0;
    }
}

void model_mutate(Model* model)
{
    double learning_rate = 0.5;

    for (size_t i = 0; i < model->weights_size; ++i) {
        Mx2* layer_weights = model->weights[i];
        Mx2* mutation = mx2_new(layer_weights->rows, layer_weights->cols);
        mx2_apply(mutation, mutation_dec);
        mx2_double_multiply(mutation, learning_rate);
        mx2_double_multiply(mutation, learning_rate / 2);
        // mx2_print(mutation);
        mx2_add(layer_weights, mutation);
        mx2_free(mutation);
    }

    for (size_t i = 0; i < model->biases_size; ++i) {
        Mx1* layer_bias = model->biases[i];
        Mx1* mutation = mx1_new(layer_bias->cols);
        mx1_apply(mutation, mutation_dec);
        mx1_double_multiply(mutation, learning_rate);
        mx1_double_multiply(mutation, learning_rate / 2);
        // mx1_print(mutation);
        mx1_add(layer_bias, mutation);
        mx1_free(mutation);
    }
}
