CC := gcc
CFLAGS := -std=c99 -Wextra -Wall -pedantic -Wvla
LDFLAGS := -lSDL2_image -lSDL2 -lm
SRC := detect.c
OBJ := $(SRC:.c=.o)
BIN := detect

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
