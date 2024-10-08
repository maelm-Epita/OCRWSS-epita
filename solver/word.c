#include <string.h>

#include "word.h"

struct point create_point(int x, int y){
  return (struct point) {x, y};
}

struct word create_word(char *content){
  struct word res;
  res.content = content;
  res.size = strlen(res.content);
  res.start = create_point(-1, -1);
  res.end = create_point(-1, -1);
  return res;
}
