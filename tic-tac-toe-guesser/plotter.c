#include "plotter.h"
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

Plotter plotter_create(char* title, char* xlabel, char* ylabel, size_t line_amount, char** labels, char** filenames)
{
    FILE** files = malloc(sizeof(FILE*) * line_amount);
    for (size_t i = 0; i < line_amount; ++i) {
        files[i] = fopen(filenames[i], "w");
    }

    return (Plotter) {
        title,
        xlabel,
        ylabel,
        line_amount,
        labels,
        filenames,
        files,
    };
}

void plotter_destroy(Plotter plotter)
{
    free(plotter.files);
}

void plotter_add_entry(Plotter plotter, size_t line_idx, size_t x, double y)
{
    fprintf(plotter.files[line_idx], "%ld %f\n", x, y);
}

void plotter_show(Plotter plotter)
{
    char* line_colors[] = { "#0060ad", "#60ad00" };
    size_t line_color_len = sizeof(line_colors) / sizeof(char*);

    for (size_t i = 0; i < plotter.line_amount; ++i) {
        fclose(plotter.files[i]);
    }

    char* command = malloc(1024);

    sprintf(command,
        "gnuplot -e \"set title '%s'; set xlabel '%s'; set ylabel '%s'; set yrange [0:0.1]; plot ",
        plotter.title, plotter.xlabel, plotter.ylabel
    );

    for (size_t i = 0; i < plotter.line_amount; ++i) {
        sprintf(command + strlen(command),
            "'%s' title '%s' linecolor rgb '%s' linewidth 2 with lines%s",
            plotter.filenames[i], plotter.labels[i], line_colors[i % line_color_len],
            i < plotter.line_amount - 1 ? ", " : ""
        );
    }

    sprintf(command + strlen(command), "; pause mouse close\"");

    printf("%s\n", command);
    system(command);
}

