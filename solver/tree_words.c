#include <err.h>
#include <stdio.h>
#include <stdlib.h>

#include "tree_words.h"

tree_word *init_tree() {
  tree_word *res = malloc(sizeof(tree_word));
  if (res == NULL)
    errx(1, "malloc()");
  *res = (tree_word){0, NULL, NULL};
  return res;
}

void add_word(tree_word *tree, char *word) {
  if (*word != '\0') {
    tree_word *old = NULL;
    while (tree != NULL && tree->c != *word) {
      old = tree;
      tree = tree->brother;
    }
    if (tree == NULL) {
      old->brother = malloc(sizeof(tree_word));
      if (old->brother == NULL)
        errx(1, "malloc()");
      tree = old->brother;
    }
    add_word(tree, word + 1);
  }
}

void destroy_aux(tree_word *tree) {
  if (tree->child != NULL) {
    destroy_aux(tree->child);
    free(tree->child);
    tree->child = NULL;
  }
  if (tree->brother != NULL) {
    destroy_aux(tree->brother);
    free(tree->brother);
    tree->brother = NULL;
  }
}

void destroy_tree(tree_word **tree) {
  destroy_aux(*tree);
  *tree = NULL;
}

void print_tree(tree_word *tree) {
  putchar('<');
  putchar(tree->c);
  tree_word *child = tree->child;
  while (child != NULL) {
    print_tree(child);
    child = child->brother;
  }
  putchar('>');
}

int is_leaf(tree_word *tree) {
  return tree->child == NULL && tree->brother == NULL;
}

int main(void) {
  tree_word *tree = init_tree();
  print_tree(tree);

  add_word(tree, "pedantic");
  print_tree(tree);

  putchar(tree->c);
  putchar(tree->brother->c);
  putchar('\n');
  return 0;
}
