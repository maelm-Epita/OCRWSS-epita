#include <err.h>
#include <stdio.h>

#include "grid.h"

int main(int argc, char *argv[]){
  if (argc != 3)
    errx(1, "usage: ./multi_solver <grid_file> <words_file>");

  FILE *grid_file = fopen(*(argv + 1), "r");

  if (grid_file == NULL)
    errx(1, "grid: file not found\n");

  grid *g = init_grid(grid_file);
  fclose(grid_file);

  print_grid(g);
  
  return 0;
}
