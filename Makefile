CC = gcc
CFLAGS = -Wall -Wextra -Wvla `pkg-config --cflags gtk+-3.0`
LDFLAGS = `pkg-config --libs gtk+-3.0` -lm -lSDL2 -lSDL2_image
SRC = nvam.c \
			GTK/*screen*.c \
			GTK/gtk_tools.c \
			GTK/draw_res.c \
			ImageProcessing/*.c \
			Detection/detection.c
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
