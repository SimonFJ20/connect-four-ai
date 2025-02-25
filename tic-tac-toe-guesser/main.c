#include "matrix.h"
#include "model.h"
#include "util.h"
#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static inline double assign_randd_dec(double v)
{
    (void)v;
    return randd_dec();
}

#define O 0.0
#define _ 0.5
#define X 1.0

int main(void)
{
    srand((uint32_t)time(NULL));

    size_t layers[] = { 9, 9, 2 };

    Model* model = malloc(sizeof(Model));
    model_contruct(model, layers, sizeof(layers) / sizeof(layers[0]));

    const double test_inputs[] = {
        // clang-format off
        X, _, O,
        _, X, O,
        O, _, X,
        // clang-format on
    };
    Mx1* inputs = mx1_from(test_inputs, 9);
    size_t test_input_size = 1;

    const double test_outputs[] = { 0.0, 1.0 };
    Mx1* correct_outputs = mx1_from(test_outputs, 2);

    size_t iterations = 100;

    Model* clone = malloc(sizeof(Model));

    printf("loss mse\ni\tmodel\tclone\n");
    for (size_t iter = 0; iter < iterations; ++iter) {
        model_clone(clone, model);
        model_mutate(clone);

        double model_acc_err = 0;
        double clone_acc_err = 0;

        for (size_t i = 0; i < test_input_size; ++i) {
            Mx1* model_outputs = model_feed(model, inputs);
            Mx1* clone_outputs = model_feed(clone, inputs);

            mx1_sub(model_outputs, correct_outputs);
            mx1_sub(clone_outputs, correct_outputs);

            model_acc_err += pow(mx1_sum(model_outputs), 2.0);
            clone_acc_err += pow(mx1_sum(clone_outputs), 2.0);

            mx1_free(model_outputs);
            mx1_free(clone_outputs);
        }

        double model_mse = model_acc_err / (double)test_input_size;
        double clone_mse = clone_acc_err / (double)test_input_size;

        if (clone_mse < model_mse) {
            model_destroy(model);
            model = clone;
        } else {
            model_destroy(clone);
        }

        printf("%ld\t%.2f\t%.2f\n", iter + 1, model_mse, clone_mse);
    }

    free(clone);

    mx1_free(correct_outputs);
    mx1_free(inputs);
    model_destroy(model);
    free(model);
}
