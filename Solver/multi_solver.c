#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "multi_solver.h"
#include "grid.h"
#include "list_word.h"
#include "tree_words.h"

grid *get_grid(char *path) {
  FILE *file = fopen(path, "r");

  if (file == NULL)
    errx(1, "grid: file not found\n");

  grid *res = init_grid(file);
  fclose(file);
  return res;
}

char **get_words(char *path, int *nb_words) {
  char **res = NULL;

  FILE *file = fopen(path, "r");
  if (file == NULL)
    errx(1, "words: file not found");

  char c;
  char *word = NULL;
  int word_size = 0;
  *nb_words = 0;

  while ((c = fgetc(file)) != EOF) {
    if (c == '\n') {
      *nb_words += 1;
      res = realloc(res, *nb_words * sizeof(char *));

      if (res == NULL)
        errx(1, "realloc()");
      word = realloc(word, (word_size + 1) * sizeof(char));
      if (word == NULL)
        errx(1, "realloc()");

      word[word_size] = '\0';
      res[*nb_words - 1] = word;
      word = NULL;
      word_size = 0;

    } else {
      word_size++;
      word = realloc(word, word_size * sizeof(char));
      if (word == NULL)
        errx(1, "realloc()");
      word[word_size - 1] = c;
    }
  }

  if (word_size > 0) {
    *nb_words += 1;
    res = realloc(res, *nb_words * sizeof(char *));

    if (res == NULL)
      errx(1, "realloc()");
    word = realloc(word, word_size + 1);
    if (word == NULL)
      errx(1, "realloc()");

    word[word_size] = '\0';
    res[*nb_words - 1] = word;
  }

  return res;
}

list_word *find_all_words(char *grid_path, char **words, size_t nb_words) {
  grid *g = get_grid(grid_path);
  print_grid(g);
  tree_word *tree = build_from_words(words, nb_words);

  list_word *list = init_list("", 0, 0, 0, 0);

  solve(g, tree, list);

  destroy_tree(&tree);
  destroy_grid(&g);
  list_word *res = list->next;
  free(list);
  return res;
}

void solve(grid *grid, tree_word *tree, list_word *res) {
  for (int i = 0; i < grid->h; i++) {
    for (int j = 0; j < grid->w; j++) {
      tree_word *child = get_child(tree, grid->letters[i * grid->w + j]);
      if (child != NULL) {
        for (int d = 0; d < 8; d++) {
          check(grid, tree, res, i, j, moves[d][1], moves[d][0]);
        }
      }
    }
  }
}

void check(grid *grid, tree_word *tree, list_word *res, int i, int j, int i_add,
           int j_add) {
  char *substring = malloc(tree_length(tree) + 1);
  if (substring == NULL)
    errx(1, "malloc()");

  tree_word *child = get_child(tree, grid->letters[i * grid->w + j]);
  int s = 0;
  int x = i + i_add, y = j + j_add;

  while (0 <= x && x < grid->h && 0 <= y && y <= grid->w && child != NULL) {
    substring[s] = child->c;
    s++;

    if (is_leaf(child)) {
      substring[s] = '\0';
      remove_word(tree, substring);

      char *str = malloc(strlen(substring) + 1);
      if (str == NULL)
        errx(1, "malloc()");
      strcpy(str, substring);

      add_element(res, str, j, i, y - j_add, x - i_add);
      child = NULL;
    }
    if (child != NULL) {
      child = get_child(child, grid->letters[x * grid->w + y]);
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
