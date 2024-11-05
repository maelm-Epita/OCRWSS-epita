CC := clang
CFLAGS := -Wextra -Wall -pedantic -Wvla -O3
SRC := multi_solver.c grid.c tree_words.c list_word.c
OBJ := $(SRC:.c=.o)
BIN := multi_solver

all: $(BIN)

solver:	SRC += solver.c
solver: OBJ += solver.o
solver:	BIN = solver
solver: $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ solver.o -o $@

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

$(OBJ):
	$(CC) $(CFLAGS) -c $(SRC)

.PHONY debug: CFLAGS += -g -fsanitize=address -O0
.PHONY debug: LDFLAGS += -fsanitize=address
.PHONY debug: $(BIN)

.PHONY: clean
clean:
	$(RM) $(OBJ) $(BIN) solver solver.o *.svg
