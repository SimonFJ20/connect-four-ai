#include "matrix.h"
#include "model.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int main(void)
{
    srand((uint32_t)time(NULL));

    size_t layers[] = { 9, 9, 2 };

    Model model;
    model_contruct(&model, layers, sizeof(layers) / sizeof(layers[0]));

    const double le_inputs[] = {
        // clang-format off
        1.0, 0.0, 0.0,
        0.0, 1.0, 0.0,
        0.0, 0.0, 1.0,
        // clang-format on
    };
    Mx1* inputs = mx1_from(le_inputs, 9);

    Mx1* outputs = model_feed(&model, inputs);

    mx1_free(inputs);
    mx1_free(outputs);
    model_destroy(&model);
}
