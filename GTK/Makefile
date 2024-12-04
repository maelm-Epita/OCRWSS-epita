CC = gcc
CFLAGS = -Wall -Wextra -Wvla -pedantic `pkg-config --cflags gtk+-3.0`
LIBS = `pkg-config --libs gtk+-3.0`
SRC = main_window.c
OBJ = $(SRC:.c=.o)
BIN = nvam-ocr

all: $(BIN)

$(BIN) : $(OBJ)
	$(CC) $(CFLAGS) $(LIBS) $^ -o $@

$(OBJ):
	$(CC) $(CFLAGS) $(LIBS) $(SRC) -c

.PHONY: clean
clean:
	$(RM) $(BIN) $(OBJ)
