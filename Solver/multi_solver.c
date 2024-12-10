#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "grid.h"
#include "list_word.h"
#include "multi_solver.h"
#include "tree_words.h"

list_word *find_all_words(grid *grid, char **words, size_t nb_words) {
  tree_word *tree = build_from_words(words, nb_words);

  list_word *list = init_list("", (letter){{0, 0}, {0, 0}, '\0'},
                              (letter){{0, 0}, {0, 0}, '\0'}, 0);

  solve_grid(grid, tree, list);

  destroy_tree(&tree);
  // destroy_grid(&grid);
  list_word *res = list->next;
  free(list);
  return res;
}

void solve_grid(grid *grid, tree_word *tree, list_word *res) {
  for (int i = 0; i < grid->h; i++) {
    for (int j = 0; j < grid->w; j++) {
      tree_word *child = get_child(tree, grid->letters[i][j].c);
      for (int d = 0; child && d < 8; d++)
        check(grid, tree, res, i, j, moves[d][1], moves[d][0], d);
    }
  }
}

void check(grid *grid, tree_word *tree, list_word *res, int i, int j, int i_add,
           int j_add, int direction) {
  char *substring = malloc(tree_length(tree) + 1);
  if (substring == NULL)
    errx(1, "malloc()");

  tree_word *child = get_child(tree, grid->letters[i][j].c);
  int s = 0;
  int x = i + i_add, y = j + j_add;

  while (0 <= x && x <= grid->h && 0 <= y && y <= grid->w && child != NULL) {
    substring[s] = child->c;
    substring[s + 1] = '\0';
    s++;

    if (is_leaf(child)) {
      substring[s] = '\0';
      remove_word(tree, substring);

      char *str = malloc(strlen(substring) + 1);
      if (str == NULL)
        errx(1, "malloc()");
      strcpy(str, substring);

      add_element(res, str, grid->letters[i][j],
                  grid->letters[x - i_add][y - j_add], direction);
      child = NULL;
    }
    if (child != NULL && x < grid->h && y < grid->w) {
      child = get_child(child, grid->letters[x][y].c);
    }
    x += i_add;
    y += j_add;
  }

  free(substring);
}

/*
int main(int argc, char *argv[]) {
  if (argc != 3)
    errx(1, "usage: ./multi_solver <grid_file> <words_file>");

  int nb_words = -1;
  char **words = get_words(argv[2], &nb_words);

  for (int i = 0; i < nb_words; i++)
    puts(words[i]);
  putchar('\n');

  list_word *res = find_all_words(argv[1], words, nb_words);

  putchar('\n');

  print_list(res);
  destroy_list(&res);

  for (int i = 0; i < nb_words; i++)
    free(words[i]);
  free(words);

  return 0;
}
*/
