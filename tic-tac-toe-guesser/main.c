#include "matrix.h"
#include "model.h"
#include "util.h"
#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define O 0.0
#define EMPTY 0.5
#define X 1.0

typedef struct {
    Mx1* input;
    Mx1* correct;
} TestDataEntry;

void test_data_generate(TestDataEntry* entries, size_t entries_size)
{
    for (size_t entries_idx = 0; entries_idx < entries_size; ++entries_idx) {
        Mx1* input = mx1_new(9);

        for (size_t j = 0; j < 9; ++j) {
            int r = rand() % 3;
            switch (r) {
                case 0:
                    *mx1_at(input, j) = 1.0;
                    break;
                case 1:
                    *mx1_at(input, j) = 0.5;
                    break;
                case 2:
                    *mx1_at(input, j) = 0.0;
                    break;
            }
        }

        Mx1* correct = mx1_new(2);

        size_t patterns[][3] = {
            // clang-format off
            { 0, 1, 2 },
            { 3, 4, 5 },
            { 6, 7, 8 },
            { 0, 4, 8 },
            { 2, 4, 6 },
            { 0, 3, 6 },
            { 1, 4, 7 },
            { 2, 5, 8 },
            // clang-format on
        };
        size_t patterns_size = sizeof(patterns) / sizeof(patterns[0]);

        for (size_t pattern_idx = 0; pattern_idx < patterns_size;
            ++pattern_idx) {
            double selected = EMPTY;
            for (size_t i = 0; i < 3; ++i) {
                double found = *mx1_at(input, patterns[pattern_idx][i]);
                if (selected != EMPTY && found != selected) {
                    selected = EMPTY;
                    break;
                }
                selected = found;
                if (found == EMPTY)
                    break;
            }
            *mx1_at(correct, 0) = selected == O ? 1.0 : 0.0;
            *mx1_at(correct, 1) = selected == X ? 1.0 : 0.0;
        }

        entries[entries_idx] = (TestDataEntry) { input, correct };
    }
}

void test_data_free(TestDataEntry* entries, size_t entries_size)
{
    for (size_t i = 0; i < entries_size; ++i) {
        mx1_free(entries[i].input);
        mx1_free(entries[i].correct);
    }
    free(entries);
}

int main(void)
{
    srand((uint32_t)time(NULL));

    size_t layers[] = { 9, 9, 2 };

    size_t training_data_size = 100;
    TestDataEntry* training_data
        = malloc(sizeof(TestDataEntry) * training_data_size);
    test_data_generate(training_data, training_data_size);

    size_t test_data_size = 100;
    TestDataEntry* test_data = malloc(sizeof(TestDataEntry) * test_data_size);
    test_data_generate(test_data, test_data_size);

    Model* model = malloc(sizeof(Model));
    Model* clone = malloc(sizeof(Model));
    model_contruct(model, layers, sizeof(layers) / sizeof(layers[0]));

    size_t iterations = 20000;
    size_t tests = 100;
    size_t iters_per_test = iterations / tests;

    FILE* train_loss_file = fopen("train_loss.dat", "w");
    FILE* test_loss_file = fopen("test_loss.dat", "w");

    printf("i\ttrain\ttest\n");
    for (size_t iter = 0; iter < iterations; ++iter) {
        model_clone(clone, model);
        model_mutate(clone);

        double model_acc_err = 0;
        double clone_acc_err = 0;

        for (size_t i = 0; i < training_data_size; ++i) {
            Mx1* model_outputs = model_feed(model, training_data[i].input);
            Mx1* clone_outputs = model_feed(clone, training_data[i].input);

            mx1_sub(model_outputs, training_data[i].correct);
            mx1_sub(clone_outputs, training_data[i].correct);

            model_acc_err += pow(mx1_sum(model_outputs), 2.0);
            clone_acc_err += pow(mx1_sum(clone_outputs), 2.0);

            mx1_free(model_outputs);
            mx1_free(clone_outputs);
        }

        double model_mse = model_acc_err / (double)training_data_size;
        double clone_mse = clone_acc_err / (double)training_data_size;

        if (clone_mse < model_mse) {
            model_destroy(model);
            Model* temp = model;
            model = clone;
            clone = temp;
        } else {
            model_destroy(clone);
        }

        if (iter % iters_per_test == 0) {

            double acc_err = 0;
            for (size_t i = 0; i < test_data_size; ++i) {
                Mx1* outputs = model_feed(model, test_data[i].input);
                mx1_sub(outputs, test_data[i].correct);
                acc_err += pow(mx1_sum(outputs), 2.0);
                mx1_free(outputs);
            }
            double mse = acc_err / (double)test_data_size;
            printf("%ld\t%.4f\t%.4f\n", iter + 1, model_mse, mse);
            fprintf(train_loss_file, "%ld %f\n", iter, model_mse);
            fprintf(test_loss_file, "%ld %f\n", iter, mse);
        }
    }

    free(clone);

    fclose(train_loss_file);
    fclose(test_loss_file);

    test_data_free(training_data, training_data_size);
    test_data_free(test_data, test_data_size);

    model_destroy(model);
    free(model);
}
