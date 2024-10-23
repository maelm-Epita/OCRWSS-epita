CC = gcc
CFLAGS = -Wall -Wextra -O3 -Wvla -fsanitize=address -g
SRC = \
			main.c \
			train/training_data_loader.c \
			train/training_data.c \
			train/training_functions.c \
			neural-net/Network.c \
			shared/math_helpers.c \
			shared/arr_helpers.c 
LFLAGS = -lSDL2 -lSDL2_image -lm

main: 
	${CC} ${SRC} ${CFLAGS} -o main $(LFLAGS)
