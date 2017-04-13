#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <stdbool.h>
#include <SDL2/SDL.h>

typedef struct graphics
{
	int src_x;
	int src_y;
	int dest_x;
	int dest_y;
	double rotation;
	bool flip_x;
} graphics;

#endif