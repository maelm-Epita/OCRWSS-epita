#include <stdlib.h>

#include "grid.h"

struct grid init_grid(FILE *file) {
  int dim[2];
  get_dimensions(file, dim);

  if (*dim == -1)
    return (struct grid){dim[0], dim[1], NULL};

  char *grid = calloc(*dim * *(dim + 1), sizeof(char));

  rewind(file);
  fill_grid(file, *dim, grid);

  if (*grid == -1)
    return (struct grid){dim[0], dim[1], NULL};

  return (struct grid){dim[0], dim[1], grid};
}

/* We get dimensions of the grid by counting
 * char by line and \n into the file */
void get_dimensions(FILE *file, int *res) {
  char c;
  int lines = 0, char_by_line = 0, nb_char = 0;

  while ((c = fgetc(file)) != EOF) {
    if (c == '\n') {
      if (lines == 0) {
        char_by_line = nb_char;
        lines++;
      } else {
        if (nb_char != char_by_line) {
          fprintf(stderr, "Invalid grid: all lines must have the same number "
                          "of characters\n");
          *res = -1;
          return;
        }
        lines++;
      }
      nb_char = 0;
    } else
      nb_char++;
  }

  *res = lines;
  *(res + 1) = char_by_line;
}

void fill_grid(FILE *file, int row, char *grid) {
  size_t i = 0, j = 0;
  char c;

  while ((c = fgetc(file)) != EOF) {
    if (c == '\n') {
      i++;
      j = 0;
    } else {
      if (c >= 'a' && c <= 'z')
        *(grid + i * row + j) = c;
      else if (c >= 'A' && c <= 'Z')
        *(grid + i * row + j) = c - 'A' + 'a';
      else {
        fprintf(stderr, "Invalid grid: grid can only contain letters\n");
        *grid = -1;
        return;
      }
      j++;
    }
  }
}

void print_grid(struct grid g) {
  puts("   0 1 2 3 4 5 6 7");
  putchar('\n');
  for (int i = 0; i < g.row; i++) {
    printf("%i  ", i);
    for (int j = 0; j < g.col; j++) {
      putchar(*(g.letters + i * g.row + j));
      putchar(' ');
    }
    putchar('\n');
  }
}
