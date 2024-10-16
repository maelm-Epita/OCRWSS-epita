#include <stdio.h>
#include <stdlib.h>
#include "arr_helpers.h"


int main(){
  size_t arr[5] = {100, 23, 45, 632, 100000};
  char* str = sze_arr_to_str(arr, 5);
  printf("%s\n", str);
  free(str);
  float farr[5] = {100.00098, 23.1, 45, 69.420, 100000.999999999999};
  str = float_arr_to_str(farr, 5);
  printf("%s\n", str);
  free(str);
}
