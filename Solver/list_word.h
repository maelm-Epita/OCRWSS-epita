#ifndef LIST_WORD_H
#define LIST_WORD_H

#include "../Detection/detect.h"
#include <stddef.h>

typedef struct list_word {
  char *word;
  point start;
  point end;
  struct list_word *next;
} list_word;

static const size_t LWSIZE = sizeof(list_word);

list_word *init_list(char *word, point start, point end);

void add_element(list_word *list, char *word, point start, point end);

void destroy_list(list_word **list);

void print_list(list_word *list);

#endif
