#include <err.h>

#include "multi_solver.h"

int main(int argc, char *argv[]) {
  if (argc != 3)
    errx(1, "Usage: ./solver <grid_path> <word>");

  list_word *res = find_all_words(argv[1], &argv[2], 1);

  if (res == NULL)
    puts("Not found");
  else
    printf("(%i,%i)(%i,%i)\n", res->start.x, res->start.y, res->end.x,
           res->end.y);

  return 0;
}
