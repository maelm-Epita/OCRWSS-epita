CC := gcc
CFLAGS := -std=c99 -Wextra -Wall -pedantic -Werror -Wvla
LDFLAGS := -lSDL2_image -lSDL2 -lm
SRC := image.c rotate.c image_tools.c matrix_maker.c
OBJ := $(SRC:.c=.o)
BIN := image

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(OBJ):
	$(CC) $(CFLAGS) -c $(SRC)

.PHONY debug: CFLAGS += -g
.PHONY debug: $(BIN)

.PHONY: clean
clean:
	$(RM) $(OBJ) $(BIN)
