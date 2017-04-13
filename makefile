CC = gcc
CFLAGS = -c \
		-std=c11 -pedantic \
		-Wall \
		-Wextra \
		-Wfloat-equal \
		-Wunreachable-code \
		$(shell sdl2-config --cflags)

LDFLAGS = $(shell sdl2-config --libs) -lSDL2_image -lm
INC = -I -Iinclude -I/home/jjwmezun/Documents/c/peppers-party/include
SRC_DIR = src/

SOURCES = \
	$(SRC_DIR)input.c \
	$(SRC_DIR)main.c \
	$(SRC_DIR)objects.c \
	$(SRC_DIR)render.c \
	$(SRC_DIR)render_magnify.c

OBJ_DIR = obj/

EXE_DIR = bin
EXE = $(EXE_DIR)/main

#################################################

all: before out

before:
	test -d $(EXE_DIR) || mkdir -p $(EXE_DIR)
	test -d $(OBJ_DIR) || mkdir -p $(OBJ_DIR)

out: $(SOURCES)
	gcc -o $(EXE) $(SOURCES) $(INC) $(LDFLAGS)

out_debug: $(SOURCES)
	gcc $(CFLAGS) $(EXE) $(SOURCES) $(INC) $(LDFLAGS)

.PHONY: clean

clean:
	rm *.o $(EXE)