#ifndef GRID_H
#define GRID_H

#include <stdio.h>


typedef struct grid {
  int w;
  int h;
  char *letters;
} grid;

grid *init_grid(FILE *file);

void get_dimensions(FILE *file, int *w, int *h);

void fill_grid(FILE *file, int row, char *grid);

void print_grid(grid *g);

#endif
