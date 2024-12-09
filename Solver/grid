#include <err.h>
#include <stdio.h>
#include <stdlib.h>

#include "grid.h"

grid *init_grid(FILE *file) {
  int w, h;
  get_dimensions(file, &w, &h);

  grid *res = malloc(sizeof(grid));
  if (res == NULL)
    errx(1, "malloc()");

  res->letters = calloc(w * h + 1, sizeof(char));
  if (res->letters == NULL)
    errx(1, "calloc()");

  res->w = w;
  res->h = h;

  rewind(file);
  fill_grid(file, res);

  return res;
}

/* We get dimensions of the grid by counting
 * char by line and \n into the file */
void get_dimensions(FILE *file, int *w, int *h) {
  char c;
  int lines = 0, char_by_line = 0, nb_char = 0;

  while ((c = fgetc(file)) != EOF) {
    if (c == '\n') {
      if (lines == 0) {
        char_by_line = nb_char;
        lines++;
      } else {
        if (nb_char != char_by_line)
          errx(1, "Invalid grid: all lines must have as much char");
        lines++;
      }
      nb_char = 0;
    } else
      nb_char++;
  }

  *h = lines;
  *w = char_by_line;
}

void fill_grid(FILE *file, grid *g) {
  size_t i = 0;
  char c;

  while ((c = fgetc(file)) != EOF) {
    if (c == '\n')
      i--;
    else if (c >= 'a' && c <= 'z')
      g->letters[i].c = c - 32;
    else if (c >= 'A' && c <= 'Z')
      g->letters[i].c = c;
    else
      errx(1, "Invalid grid: grid can only contain letters");
    i++;
  }
  g->letters[g->w * g->h].c = '\0';
}

void destroy_grid(grid **g) {
  free((*g)->letters);
  free(*g);
  *g = NULL;
}

void print_grid(grid *g) {
  putchar(' ');
  putchar(' ');
  for (int i = 0; i < g->w; i++)
    printf(" %i", i);
  putchar('\n');

  putchar('\n');
  for (int i = 0; i < g->h; i++) {
    printf("%i  ", i);
    for (int j = 0; j < g->w; j++) {
      putchar(g->letters[i * g->w + j].c);
      putchar(' ');
    }
    putchar('\n');
  }
}
