#ifndef TREE_WORDS_H
#define TREE_WORDS_H

#include <stddef.h>


typedef struct tree_word {
  char c;
  struct tree_word* child;
  struct tree_word* brother;
} tree_word;

tree_word *init_tree(char c);


static const size_t TWSIZE = sizeof(tree_word);


void add_word(tree_word *tree, char *word);

void remove_word(tree_word *tree, char *word);

void destroy_tree(tree_word **tree);

size_t nb_children(tree_word *tree);

tree_word *get_child(tree_word *tree, char c);

int is_leaf(tree_word *tree);


void print_tree(tree_word *tree);

void export_tree(tree_word *tree, char *name);



#endif
