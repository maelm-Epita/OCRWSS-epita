#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "training_data_loader.h"

#define OUTPUT_SIZE 26

// helpers
size_t str_to_size(char* str){
  size_t s = 0;
  for (size_t i=0; *(str+i)!=0; i++){
    s*=10;
    s+=*(str+i)-'0';
  }
  return s;
}

double* snum_to_output(size_t n){
  double* output=calloc(OUTPUT_SIZE, sizeof(double));
  if (output==NULL){
    printf("could not create output array");
    exit(EXIT_FAILURE);
  }
  *(output+(n)) = 1; 
  return output;
}

void load_training_data(char* path, double** *inputs_p, double** *outputs_p, size_t data_number, size_t input_size){
  FILE *file;
  file = fopen(path, "r");
  if (file == NULL) {
    errx(EXIT_FAILURE, "File could not be opened\n");
  }
  size_t line_size_max = 100000;
  char buf;
  size_t line_nb = 0;
  // free input and output if they alrdy exist
  if (*inputs_p != NULL){
    free(*inputs_p);
  }
  if (*outputs_p != NULL){
    free(*outputs_p);
  }
  // we alloc depending on the number of training data
  *inputs_p = calloc(data_number, sizeof(double*));
  if (*inputs_p == NULL){
    printf("could not alloc input array");
    exit(EXIT_FAILURE);
  }
  *outputs_p = calloc(data_number, sizeof(double*));
  if (*outputs_p == NULL){
    printf("could not alloc output array");
    exit(EXIT_FAILURE);
  }
  // load each line until data_number lines have been loaded
  while (line_nb<data_number){
    char* line = calloc(line_size_max, sizeof(char));
    if (line == NULL){
      printf("could not allocate line\n");
      exit(EXIT_FAILURE);
    }
    buf = 0;
    size_t linesize = 0;
    // read the file char by char until the end of the line
    while (linesize<line_size_max && buf!='\n' && buf != EOF){
      buf = fgetc(file);
      if (buf != '\n' && buf != EOF){
          *(line+linesize) = buf;
        }
        // if the end of line is found we end the string
      else{
          *(line+linesize) = 0;
      }
      linesize++;
    }
    // reajust size of the line properly
    line = realloc(line, linesize);
    if (line == NULL){
      printf("could not realloc line");
      exit(EXIT_FAILURE);
    }
    double *input;
    double *output;
    if (linesize>1){
      // if we are loading the handwritten training set ; the lines are : expected label index -- pixels
      if (strcmp(path, "./training-set/handwritten_letters.csv") == 0 || strcmp(path, "./training-set/easy_font.csv") == 0 ||
          strcmp(path, "./training-set/hard_font.csv") == 0) {
        // get the expected output of the training data; we get the line until the first , 
        // then convert that to a number then get the output from that number
        output = snum_to_output(str_to_size(strtok(line, ",")));
        // get the remaining input_size columns which are the pixels and convert them to a number then from 0-255 to 0-1
        input = calloc(input_size, sizeof(double));
        for (size_t i = 0; i<input_size; i++){
          // the last pixel does not have a , to separate it, it's the end of the line or of the file
          // so we pass nothing to strtok to get the remainder of the string
          if (i==input_size-1){
            *(input+i) = atof(strtok(NULL, ""))/255;
          }
          else{
            *(input+i) = atof(strtok(NULL, ","))/255;
          }
        }
      }
      // if we are loading the digital training set ; the lines are : pixels -- expected letter guess
      else if (strcmp(path, "./training-set/digital_letters.csv") == 0 || 
          strcmp(path, "./training-set/digital_letters_alt.csv") == 0)
      {
        // the first element in the line is the index (data number) so we just discard it
        strtok(line, ",");
        // get the first input_size columns which are the pixels and convert them to a number then from 0-255 to 0-1
        input = calloc(input_size, sizeof(double));
        for (size_t i = 0; i<input_size; i++){
          *(input+i) = atof(strtok(NULL, ","))/255;
        }
        // get the expected output of the training data; we get the line until the first ,
        // then convert that to a number then get the output from that number
        // as in the other format, the last element in the line (the label here)
        output = snum_to_output(*(strtok(NULL, ""))-'A');
      }
      else{
        errx(EXIT_FAILURE, "Unknown training set path : %s, can't infer formatting.\n", path);
      }
      if (input == NULL || output == NULL){
        errx(EXIT_FAILURE, "Failed to load the data for line %lu\n", line_nb);
      }
      // we get the correct input and output from the data
      *(*inputs_p+line_nb) = input;
      *(*outputs_p+line_nb) = output;
    }
    // free the strings
    free(line);
    line_nb++;
  }
  fclose(file);
}
