CC = clang
CFLAGS = -Wall -Wextra -Wvla -Ofast `pkg-config --cflags gtk+-3.0`
LDFLAGS = `pkg-config --libs gtk+-3.0` -lm -lSDL2 -lSDL2_image
SRC = nvam.c \
			GTK/*screen*.c \
			GTK/gtk_tools.c \
			GTK/draw_res.c \
			ImageProcessing/*.c \
			Detection/detect.c \
			Detection/detection.c \
			Solver/*.c \
			DeepLearning/neural-net/Network.c \
			DeepLearning/neural-net/network_functions.c \
			DeepLearning/shared/math_helpers.c \
			DeepLearning/shared/arr_helpers.c 
OBJ = *.o
BIN = nvam-ocr

all: $(BIN)

$(BIN) : $(OBJ)
	$(CC) $(LDFLAGS) $^ -o $@

$(OBJ):
	$(CC) $(CFLAGS) $(SRC) -c

.PHONY: debug
debug: CFLAGS += -g -O0 -fsanitize=address
debug: LDFLAGS += -g -O0 -fsanitize=address
debug: $(BIN)

.PHONY: clean
clean:
	$(RM) $(BIN) $(OBJ)
