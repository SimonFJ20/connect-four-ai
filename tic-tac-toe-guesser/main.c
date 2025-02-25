#include "matrix.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    size_t* layers;
    size_t layers_size;

    Mx2** weights;
    size_t weights_size;

    Mx1** biases;
    size_t biases_size;
} Model;

int model_contruct(Model* model, size_t* layers, size_t layers_size)
{
    size_t weights_size = layers_size - 1;
    Mx2** weights = malloc(weights_size);

    size_t biases_size = layers_size - 1;
    Mx1** biases = malloc(biases_size);

    for (size_t i = 0; i < layers_size - 1; i++) {
        weights[i] = mx2_new(layers[i], layers[i + 1]);
        biases[i] = mx1_new(layers[i + 1]);
    }

    *model = (Model) {
        layers,
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
    free(model->layers);
    free(model->weights);
    free(model->biases);
}

Mx1* model_feed(Model* model, Mx1* inputs)
{
    Mx1* outputs = inputs;

    size_t layers_idcs = model->layers_size - 1;
    for (size_t layer_idx = 0; layer_idx < layers_idcs; ++layer_idx) {
        //
    }
    return outputs;
}

int main(void)
{
    size_t layers[] = { 9, 9, 2 };

    Model model;
    model_contruct(&model, layers, sizeof(layers) / sizeof(layers[0]));

    model_destroy(&model);
}
