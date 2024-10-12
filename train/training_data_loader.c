#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>

// helpers
char to_upper(char c){
  if (c>='a' && c<='z'){
    c = c + ('A'-'a');
  }
  return c;
}
size_t str_len_with_null(char* str){
  size_t size =0;
  while (*(str+size)!=0){
    size++;
  }
  return size+1;
}
char* str_until(size_t size, char* str, char sep){
  char* res = calloc(size, sizeof(char));
  size_t rsize = 0;
  char b;
  while (rsize<size && b != sep){
    b = *(str+rsize);
    if (b != sep){
      *(res+rsize) = b;
    }
    else{
      *(res+rsize) = 0;
    }
    rsize++;
  }
  res = realloc(res, rsize);
  return res;
}
char* str_from(size_t size, char* str, char sep){
  char* res = calloc(size, sizeof(char));
  char found_sep = 0;
  size_t rsize = 0;
  size_t i = 0;
  char b = 1;
  while (i<size && b != 0){
    b = *(str+i);
    if (b == sep){
      found_sep = 1;
    }
    else{
      if (found_sep){
        *(res+rsize) = b;
        rsize++;
      }
    }
    i++;
  }
  res = realloc(res, rsize);
  return res;
}
size_t str_to_size(char* str){
  size_t s = 0;
  for (size_t i=0; *(str+i)!=0; i++){
    s*=10;
    s+=*(str+i)-'0';
  }
  return s;
}

// property ish
size_t get_outputs_size(){
  return 26;
}


float* image_to_input(char* path){
  SDL_Surface *img = IMG_Load(path);
  if (img == NULL){
    printf("could not open image\n");
    exit(EXIT_FAILURE);
  }
  SDL_LockSurface(img);
  SDL_PixelFormat *format = img->format;
  Uint32* pixels = img->pixels;
  size_t size = img->w * img->h;
  float* input = calloc(size, sizeof(float));
  if (input==NULL){
    printf("could not create input array");
    exit(EXIT_FAILURE);
  }
  for (size_t i=0; i<size;i++){
    Uint8 r;
    Uint8 g;
    Uint8 b;
    Uint32 pixel = *(pixels+i);
    SDL_GetRGB(pixel, format, &r, &g, &b);
    float greyscale = (float)(r+g+b)/3.0;
    *(input+i) = greyscale;
  }
  SDL_FreeSurface(img);
  return input;
}

float* alpha_to_output(char letter){
  letter = to_upper(letter);
  float* output = calloc(get_outputs_size(), sizeof(float));
  if (output==NULL){
    printf("could not create output array");
    exit(EXIT_FAILURE);
  }
  *(output+(letter-'A')) = 1; 
  return output;
}

void load_training_data(FILE *file, float** *inputs_p, float** *outputs_p, size_t *data_number, size_t *input_number ){
  size_t line_size_max = 150;
  char buf;
  size_t line_nb = 0;
  size_t data_nb;
  // until end of file
  while (buf != EOF){
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
    if (linesize>1){
      // get first half of line 
      char* first = str_until(linesize, line, ' ');
      // get second half of line
      char* second = str_from(linesize, line, ' ');
      // if first line
      if (line_nb==0){
        // we return the number of data to the pointer 
        data_nb = str_to_size(first);
        *data_number = data_nb;
        // we return the input size (total number of pixels in one image)
        char *ch = str_until(str_len_with_null(second), second, 'x');
        char *cw = str_from(str_len_with_null(second), second, 'x');
        *input_number = str_to_size(ch)*str_to_size(cw);
        if (*inputs_p != NULL){
          free(*inputs_p);
        }
        if (*outputs_p != NULL){
          free(*outputs_p);
        }
        // we alloc depending on the number of training data
        *inputs_p = calloc(data_nb, sizeof(float*));
        if (*inputs_p == NULL){
          printf("could not alloc input array");
          exit(EXIT_FAILURE);
        }
        *outputs_p = calloc(data_nb, sizeof(float*));
        if (*outputs_p == NULL){
          printf("could not alloc output array");
          exit(EXIT_FAILURE);
        }
        // free the strings
        free(ch);
        free(cw);
      }
      else{
        // we get the correct input and output from the data
        *(*inputs_p+line_nb-1) = image_to_input(first);
        *(*outputs_p+line_nb-1) = alpha_to_output(*second);
      }
      // free the strings
      free(second);
      free(first);
    }
    free(line);
    line_nb++;
  }
}
