#include <stdio.h>

typedef struct {
	char* title;
	char* xlabel;
	char* ylabel;
	size_t line_amount;
	char** labels;
	char** filenames;
	FILE** files;
} Plotter;

Plotter plotter_create(char* title, char* xlabel, char* ylabel, size_t line_amount, char** labels, char** filenames);
void plotter_destroy(Plotter plotter);
void plotter_add_entry(Plotter plotter, size_t line_idx, size_t x, double y);
void plotter_show(Plotter plotter);

