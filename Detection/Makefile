CC := gcc
CFLAGS := -std=c99 -Wextra -Wall -pedantic -Wvla
LDFLAGS := -lSDL2_image -lSDL2 -lm
SRC_DETECT := detect.c
OBJ_DETECT := $(SRC_DETECT:.c=.o)
BIN_DETECT := detect
SRC_DETECTION := detection.c
OBJ_DETECTION := $(SRC_DETECTION:.c=.o)
BIN_DETECTION := detection

detection: $(BIN_DETECTION)

$(BIN_DETECTION): $(OBJ_DETECTION)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(OBJ_DETECTION):
	$(CC) $(CFLAGS) -c $(SRC_DETECTION)

detect: $(BIN_DETECT)

$(BIN_DETECT): $(OBJ_DETECT)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(OBJ_DETECT):
	$(CC) $(CFLAGS) -c $(SRC_DETECT)

.PHONY debug: CFLAGS += -g -O0 -fsanitize=address
.PHONY debug: $(BIN_DETECT)

.PHONY: clean
clean:
	$(RM) $(OBJ_DETECT) $(BIN_DETECT) $(OBJ_DETECTION) $(BIN_DETECTION)
