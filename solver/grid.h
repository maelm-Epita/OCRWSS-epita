#ifndef GRID_H
#define GRID_H

#include <stdio.h>


struct grid {
  int row;
  int col;
  char *letters;
};

struct grid init_grid(FILE *file);

void get_dimensions(FILE *file, int *res);

void fill_grid(FILE *file, int row, char *grid);

void print_grid(struct grid g);

#endif
