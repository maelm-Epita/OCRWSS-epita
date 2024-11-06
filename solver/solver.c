#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "grid.h"
#include "word.h"

#include "solver.h"

int main(int argc, char *argv[]) {
  if (argc != 3) {
    fprintf(stderr, "use solver : solver <grid file> <word to find>\n");
    return 1;
  }

  struct word w = create_word(*(argv + 2));
  FILE *grid_file = fopen(*(argv + 1), "r");

  if (grid_file == NULL) {
    fprintf(stderr, "grid: file not found\n");
    return 1;
  }

  struct grid g = init_grid(grid_file);
  fclose(grid_file);
  if (g.letters == NULL)
    return -1;

  print_grid(g);
  putchar('\n');

  solver(g, &w);
  printf("(%i, %i) -> (%i, %i)\n", w.start.x, w.start.y, w.end.x, w.end.y);

  return 0;
}

void solver(struct grid g, struct word *w) {
  for (int i = 0; i < g.row; i++) {
    for (int j = 0; j < g.col; j++) {
      if (*(g.letters + i * g.row + j) == *w->content) {
        int find = 0;
        int top = i - w->size + 1 >= 0;
        int bottom = i + w->size <= g.row;
        int right = j + w->size <= g.col;
        int left = j - w->size + 1 >= 0;

        // TOP search
        if (top)
          find = search(j, i, 0, -1, g, w);
        // RIGHT search
        if (!find && right)
          find = search(j, i, 1, 0, g, w);
        // BOTTOM search
        if (!find && bottom)
          find = search(j, i, 0, 1, g, w);
        // LEFT search
        if (!find && left)
          find = search(j, i, -1, 0, g, w);
        // TOP-RIGHT search
        if (!find && top && right)
          find = search(j, i, 1, -1, g, w);
        // BOTTOM-RIGHT search
        if (!find && bottom && right)
          find = search(j, i, 1, 1, g, w);
        // BOTTOM-LEFT search
        if (!find && bottom && left)
          find = search(j, i, -1, 1, g, w);
        // TOP-LEFT search
        if (!find && top && left)
          find = search(j, i, -1, -1, g, w);

        if (find) {
          w->start = create_point(j, i);
          return;
        }
      }
    }
  }
  puts("Not found");
}

int search(int x, int y, int addx, int addy, struct grid g, struct word *w) {
  int k = 0;
  while (*(w->content + k) != '\0' &&
         *(g.letters + y * g.row + x) == *(w->content + k)) {
    k++;
    x += addx;
    y += addy;
  }
  if (k == w->size) {
    x -= addx;
    y -= addy;
    w->end = create_point(x, y);
  }
  return k == w->size;
}
