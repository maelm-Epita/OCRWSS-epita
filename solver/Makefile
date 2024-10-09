CC := gcc
CFLAGS := -Wextra -Wall -pedantic -Werror -Wvla
SRC := multi_solver.c grid.c tree_words.c list_word.c
OBJ := $(SRC:.c=.o)
BIN := multi_solver

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(OBJ):
	$(CC) $(CFLAGS) -c $(SRC)

.PHONY debug: CFLAGS += -g -fsanitize=address
.PHONY debug: LDFLAGS += -fsanitize=address
.PHONY debug: $(BIN)

.PHONY: clean
clean:
	$(RM) $(OBJ) $(BIN)
