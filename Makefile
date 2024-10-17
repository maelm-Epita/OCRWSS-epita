CC = gcc
CFLAGS = -Wall -Wextra -O3 -Wvla -fsanitize=address -g
SRC = \
			main.c \
			train/training_data_loader.c \
			train/training_data.c \
			neural-net/Network.c \
			neural-net/math_helpers.c \
			neural-net/arr_helpers.c 
LFLAGS = -lSDL2 -lSDL2_image -lm

main: 
	${CC} ${SRC} ${CFLAGS} -o main $(LFLAGS)
