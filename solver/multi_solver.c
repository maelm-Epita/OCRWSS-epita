#include <err.h>
#include <stdio.h>
#include <stdlib.h>

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
  tree_word *tree = init_tree('\0');
  for (size_t i = 0; i < nb_words; i++)
    add_word(tree, words[i]);
  export_tree(tree, "tree");

  destroy_tree(&tree);
  destroy_grid(&g);
  return NULL;
}

int main(int argc, char *argv[]) {
  if (argc != 3)
    errx(1, "usage: ./multi_solver <grid_file> <words_file>");

  int nb_words = -1;
  char **words = get_words(argv[2], &nb_words);

  for (int i = 0; i < nb_words; i++)
    puts(words[i]);

  list_word *res = find_all_words(argv[1], words, nb_words);

  return 0;
}
