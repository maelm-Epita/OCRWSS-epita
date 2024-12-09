#ifndef GRID_H
#define GRID_H

#include <stdio.h>
#include "../Detection/detect.h"

typedef struct grid {
  int w;
  int h;
  letter **letters;
} grid;

//grid *init_grid(FILE *file);
//
//void get_dimensions(FILE *file, int *w, int *h);
//
//void fill_grid(FILE *file, grid *g);
//
//void print_grid(grid *g);
//
//void destroy_grid(grid **g);

#endif
