CC := gcc
CFLAGS := -std=c99 -Wextra -Wall -pedantic -Werror -Wvla
LDFLAGS := -lSDL2_image -lSDL2 -lm
SRC := detection.c
OBJ := $(SRC:.c=.o)
BIN := detection

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(OBJ):
	$(CC) $(CFLAGS) -c $(SRC)

.PHONY debug: CFLAGS += -g -O0 -fsanitize=address
.PHONY debug: $(BIN)

.PHONY: clean
clean:
	$(RM) $(OBJ) $(BIN)
