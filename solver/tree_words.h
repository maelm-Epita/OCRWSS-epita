#ifndef TREE_WORDS_H
#define TREE_WORDS_H

typedef struct tree_word {
  char c;
  struct tree_word* child;
  struct tree_word* brother;
} tree_word;

tree_word *init_tree();

void add_word(tree_word *tree, char *word);

void destroy_tree(tree_word **tree);

void print_tree(tree_word *tree);


int is_leaf(tree_word *tree);

#endif
