#ifndef word_H
#define WORD_H

struct point {
  int x;
  int y;
};

struct word {
  struct point start;
  struct point end;
  int size;
  char *content;
};

struct point create_point(int x, int y);
struct word create_word(char *content);

#endif
