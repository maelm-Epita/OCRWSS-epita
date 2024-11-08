CC = clang
CFLAGS = -Wall -Wextra -Werror -Wvla -fsanitize=address -g -D_GNU_SOURCE
SPEED := -O3 -Ofast -flto -funroll-loops -fstrict-aliasing -ftree-vectorize -fomit-frame-pointer -march=native -mtune=native
SRC = \
			main.c \
			train/training_data_loader.c \
			train/training_data.c \
			train/training_functions.c \
			neural-net/Network.c \
			shared/math_helpers.c \
			shared/arr_helpers.c 
LFLAGS = -lSDL2 -lm

all: 
	${CC} ${SRC} ${CFLAGS} ${SPEED} -o main $(LFLAGS)
.PHONY: clean
clean:
	rm main
