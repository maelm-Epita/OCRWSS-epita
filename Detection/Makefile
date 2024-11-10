CC := gcc
CFLAGS := -Wextra -Wall -Werror -Wvla  -D_USE_MATH_DEFINES
LDFLAGS := -lSDL2_image -lSDL2 -lm  -D_USE_MATH_DEFINES
SRC := detection.c hough.c
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
