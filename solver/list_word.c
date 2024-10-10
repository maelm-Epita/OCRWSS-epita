#include <err.h>
#include <stdio.h>
#include <stdlib.h>

#include "list_word.h"

list_word *init_list(char *word, int startx, int starty, int endx, int endy) {
  list_word *res = malloc(LWSIZE);
  if (res == NULL)
    errx(1, "malloc()");
  res->word = word;
  res->start = (point){startx, starty};
  res->end = (point){endx, endy};
  res->next = NULL;

  return res;
}

void add_element(list_word *list, char *word, int startx, int starty, int endx,
                 int endy) {
  while (list->next != NULL)
    list = list->next;
  list->next = init_list(word, startx, starty, endx, endy);
}

void destroy_list(list_word **list) {
  list_word *curr = *list;
  list_word *old = NULL;
  while (curr != NULL) {
    old = curr;
    curr = curr->next;
    free(old);
  }
  *list = NULL;
}

void print_list(list_word *list) {
  if (list != NULL) {
    printf("%s (%i, %i) -> (%i, %i)\n", list->word, list->start.x,
           list->start.y, list->end.x, list->end.y);
    if (list->next != NULL)
      print_list(list->next);
  }
}
