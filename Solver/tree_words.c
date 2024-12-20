#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>

#include "tree_words.h"

tree_word *init_tree(char c) {
  tree_word *res = malloc(TWSIZE);
  if (res == NULL)
    errx(1, "malloc()");
  *res = (tree_word){c, NULL, NULL};
  return res;
}

void add_aux(tree_word *tree, char *word) {
  if (*word != '\0') {
    tree_word *new_tree = tree;
    tree_word *old = NULL;
    while (new_tree != NULL && new_tree->c != *word) {
      old = new_tree;
      new_tree = new_tree->brother;
    }
    if (new_tree == NULL) {
      old->brother = init_tree(*word);
      new_tree = old->brother;
    }
    add_word(new_tree, word + 1);
  }
}
void add_word(tree_word *tree, char *word) {
  if (tree->child == NULL) {
    tree_word *child = tree;
    for (size_t i = 0; word[i] != '\0'; i++) {
      child->child = init_tree(word[i]);
      child = child->child;
    }
  } else
    add_aux(tree->child, word);
}

tree_word *build_from_words(char **words, int nb_words){
  tree_word *res = init_tree('\0');
  for (int i = 0; i < nb_words; i++)
    add_word(res, words[i]);
  return res;
}



void remove_word(tree_word *tree, char *word) {
  tree_word *child = get_child(tree, *word);
  if (word[1] != 0 && child != NULL)
    remove_word(child, word + 1);

  if (child != NULL && child->child == NULL) {
    if (child == tree->child)
      tree->child = child->brother;
    else {
      tree_word *prev = tree->child;
      while (prev->brother != child)
        prev = prev->brother;
      prev->brother = child->brother;
    }
    free(child);
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
  free(*tree);
  *tree = NULL;
}

size_t nb_children(tree_word *tree){
  size_t res = 0;
  tree_word *child = tree->child;
  while (child != NULL){
    res++;
    child = child->brother;
  }
  return res;
}

tree_word *get_child(tree_word *tree, char c) {
  tree_word *res = tree->child;
  while (res != NULL && res->c != c)
    res = res->brother;
  return res;
}

int is_leaf(tree_word *tree) {
  return tree->child == NULL && tree->brother == NULL;
}

int tree_length(tree_word *tree){
  int res = 0;
  tree_word *child = tree->child;
  while (child != NULL){
    int len = tree_length(child);
    res = len > res ? len : res;
    child = child->brother;
  }
  return res + 1;
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

char *export_aux(tree_word *tree, int nb) {
  char *old_res = NULL;
  char *res = NULL;
  if (tree->c != 0)
    asprintf(
        &old_res,
        "\tnode [fillcolor=white shape=circle style=filled]\n\t%i [label=%c]\n",
        nb, tree->c);
  else
    asprintf(&old_res,
             "\tnode [fillcolor=white shape=circle style=filled]\n\t%i "
             "[label=\"\"]\n",
             nb);

  tree_word *child = tree->child;
  int child_nb = 2 * nb;
  while (child != NULL) {
    char *aux = export_aux(child, child_nb);
    asprintf(&res, "%s%s\t%i -> %i\n", old_res, aux, nb, child_nb);
    free(aux);
    free(old_res);
    old_res = res;

    child = child->brother;
    child_nb = child_nb * 2 + 1;
  }

  return res == NULL ? old_res : res;
}

void export_tree(tree_word *tree, char *name) {
  char *res = "digraph {\n\tgraph [rankdir=TB]\n";
  char *aux = export_aux(tree, 1);
  asprintf(&res, "%s%s%s", res, aux, "}\n");
  free(aux);

  FILE *dot_file = fopen(name, "w");
  fprintf(dot_file, "%s", res);
  fclose(dot_file);

  int pid = fork();
  if (pid == -1)
    errx(1, "fork()");
  else if (pid == 0) {
    char *argv[4] = {
        "dot",
        "-Tsvg",
        name,
        "-O",
    };
    execvp(argv[0], argv);
    kill(getpid(), SIGKILL);
  }

  free(res);
}


/*
int main(void) {
  tree_word *tree = init_tree('\0');
  // print_tree(tree);

  add_word(tree, "HORSE");
  add_word(tree, "PIG");
  add_word(tree, "GOAT");
  add_word(tree, "CHICK");
  add_word(tree, "DUCK");
  add_word(tree, "SHEEP");
  add_word(tree, "COW");
  add_word(tree, "DOG");
  add_word(tree, "CAT");
  print_tree(tree);

  putchar('\n');
  export_tree(tree, "farm");
  remove_word(tree, "DOG");
  remove_word(tree, "SHEEP");
  remove_word(tree, "PIG");
  remove_word(tree, "CAT");
  remove_word(tree, "DUCK");
  remove_word(tree, "COW");
  remove_word(tree, "CHICK");
  remove_word(tree, "GOAT");
  remove_word(tree, "HORSE");
  remove_word(tree, "COW");
  export_tree(tree, "farm2");

  destroy_tree(&tree);
  return 0;
}
*/
