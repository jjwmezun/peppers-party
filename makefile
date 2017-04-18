CC = gcc
CFLAGS = \
		-std=c11 -pedantic \
		-Wall \
		-Wextra \
		-Wfloat-equal \
		-Wunreachable-code \
		$(shell sdl2-config --cflags)

LDFLAGS = $(shell sdl2-config --libs) -lSDL2_image -lm
INC = -I -Isrc/ -I/home/jjwmezun/Documents/c/peppers-party/src
SRC_DIR = src/

SOURCES = $(wildcard $(SRC_DIR)*.c) $(wildcard $(SRC_DIR)**/*.c)

EXE_DIR = bin
EXE = $(EXE_DIR)/main

#################################################

all: before out

before:
	test -d $(EXE_DIR) || mkdir -p $(EXE_DIR)
	test -d $(OBJ_DIR) || mkdir -p $(OBJ_DIR)

out: $(SOURCES)
	gcc $(CFLAGS) -o $(EXE) $(SOURCES) $(INC) $(LDFLAGS)

.PHONY: clean

clean:
	rm $(EXE)