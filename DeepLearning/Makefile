CC = clang
CFLAGS = -Wall -Wextra -Werror -Wvla -D_GNU_SOURCE
LDFLAGS = -lSDL2 -lm
SPEED := -Ofast -march=native -mtune=native -mavx2 

SRC = \
			main.c \
			train/training_data_loader.c \
			train/training_data.c \
			train/training_functions.c \
			neural-net/Network.c \
			shared/math_helpers.c \
			shared/arr_helpers.c 

OBJ = main.o training_data_loader.o training_data.o training_functions.o Network.o math_helpers.o arr_helpers.o
BIN = main


all: $(BIN)

$(BIN): $(OBJ)
	$(CC) $^ $(SPEED) -o $@ $(LDFLAGS)

$(OBJ):
	$(CC) $(CFLAGS) $(SPEED) -c $(SRC)

debug: CFLAGS += -fsanitize=address -g
debug: LDFLAGS += -fsanitize=address -g
debug: $(BIN)

.PHONY: clean
clean:
	$(RM) $(BIN) $(OBJ)
