CC = gcc
CFLAGS = -Wall -Wextra -O1 -Wvla -fsanitize=address -g
SRC = training_data_loader.c training_data.c test.c
LFLAGS = -lSDL2 -lSDL2_image

main: 
	${CC} ${SRC} ${CFLAGS} -o main $(LFLAGS)
