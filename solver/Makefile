CC := gcc
CFLAGS := -std=c99 -Wextra -Wall -pedantic -Werror -Wvla
SRC := solver.c grid.c word.c
OBJ := $(SRC:.c=.o)
BIN := solver

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
