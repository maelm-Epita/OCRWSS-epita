#ifndef LIST_WORD_H
#define LIST_WORD_H

#include "../Detection/detect.h"
#include <stddef.h>

enum directions {
  right,
  down,
  left,
  up,
  up_right,
  down_right,
  down_left,
  up_left
};

typedef struct list_word {
  char *word;
  letter start;
  letter end;
  enum directions direction;
  struct list_word *next;
} list_word;

static const size_t LWSIZE = sizeof(list_word);

list_word *init_list(char *word, letter start, letter end, enum directions direction);

void add_element(list_word *list, char *word, letter start, letter end, enum directions direction);

void destroy_list(list_word **list);

void print_list(list_word *list);

#endif
