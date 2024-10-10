#ifndef MULTI_SOLVER_H
#define MULTI_SOLVER_H

#include <err.h>
#include <stdlib.h>

#include "grid.h"
#include "list_word.h"

grid *get_grid(char *path);

char **get_words(char *path, int *nb_words);

list_word *find_all_words(char *grid_path, char **words, size_t nb_words);

enum directions {
  right,
  down,
  left,
  up,
  up_right,
  down_right,
  down_left,
  up_left
};
static int moves[8][2] = {{1, 0},  {0, 1}, {-1, 0}, {0, -1},
                          {1, -1}, {1, 1}, {-1, 1}, {-1, -1}};

#endif
