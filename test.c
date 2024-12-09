#include <stdio.h>
#include <stdlib.h>

void print_arr(int* arr, int size){
  printf("[ ");
  for (int i=0; i<size; i++){
    if (i==size-1){
      printf("%d", arr[i]);
    }
    else{
      printf("%d, ", arr[i]);
    }
  }
  printf(" ]\n");
}
void print_mat(int** mat, int size_x, int size_y){
  for (int i=0; i<size_y; i++){
    print_arr(mat[i], size_x);
  }
}

void order_with_threshold(int* arr, int size, int threshold){
  for (int i=0; i<size; i++){
    int min_i = i;
    for (int j=i+1; j<size; j++){
      if (arr[j] < arr[min_i] - threshold){
        min_i = j;
      }
    }
    int temp = arr[i];
    arr[i] = arr[min_i];
    arr[min_i] = temp;
  }
}

void sorted_to_matrix(int* sorted_arr, int size, int threshold, int** *matrix, int* mat_size_x, int* mat_size_y){
  // divide the sorted cells into different lines of a matrix
  int current_level = 0;
  int current_el = 0;
  int** mat = NULL;
  for (int i=0; i<size; i++){
    int el = sorted_arr[i];
    // if first element of the array we initialize
    if (i==0){
      mat = realloc(mat, (current_level+1)*sizeof(int*));
      mat[current_level] = NULL;
      mat[current_level] = realloc(mat[current_level], (current_el+1)*sizeof(int));
      mat[current_level][current_el] = el;
      current_el++;
    }
    // if we are on any other element
    else{
      // if we are on a different y level with threshold
      printf("el: %d, prev: %d\n", el, mat[current_level][current_el-1]);
      if (el - threshold > mat[current_level][current_el-1]){
        // we alloc another level
        // we are now on the 0th el of this new level
        current_level++;
        current_el = 0;
        mat = realloc(mat, (current_level+1)*sizeof(int*));
        mat[current_level] = NULL;
      }
      // we add the element at the current level as the current element
      mat[current_level] = realloc(mat[current_level], (current_el+1)*sizeof(int));
      mat[current_level][current_el] = el;
      current_el++;
    }
  }
  // we will end on the last element of the last level thus we have the dimensions
  *mat_size_y = current_level+1;
  *mat_size_x = current_el;
  *matrix = mat;
}

int main(){
  int test[8] = {45, 10, 56, 200, 94, 90, 15, 205};
  print_arr(test, 8);
  order_with_threshold(test, 8, 20);
  print_arr(test, 8);
  int size_x;
  int size_y;
  int** mat;
  sorted_to_matrix(test, 8, 20, &mat, &size_x, &size_y);
  print_mat(mat, size_x, size_y);
}
