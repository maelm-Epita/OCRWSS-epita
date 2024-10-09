#ifndef LIST_WORD_H
#define LIST_WORD_H

#include <stddef.h>

typedef struct point{
  int x, y;
} point;

typedef struct list_word{
  char *word;
  point start;
  point end;
  struct list_word *next;
} list_word;


const size_t LWSIZE = sizeof(list_word);


list_word *init_list(char *word, int startx, int starty, int endx, int endy);

void add_element(list_word *list, char *word, int startx, int starty, int endx, int endy);

void destroy_list(list_word **list);

void print_list(list_word *list);

#endif
