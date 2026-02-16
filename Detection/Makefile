CC := gcc
CFLAGS := -std=c99 -Wextra -Wall -pedantic -Wvla -D_GNU_SOURCE
LDFLAGS := -lSDL2_image -lSDL2 -lm
SRC_DETECT := detect.c \
							../DeepLearning/neural-net/Network.c \
							../DeepLearning/neural-net/network_functions.c \
							../DeepLearning/shared/math_helpers.c \
							../DeepLearning/shared/arr_helpers.c \
							../ImageProcessing/image_tools.c \
							../ImageProcessing/resize.c
OBJ_DETECT := detect.o \
							Network.o \
							network_functions.o \
							math_helpers.o \
							arr_helpers.o \
							image_tools.o \
							resize.o
							
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
