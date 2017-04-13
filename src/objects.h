#ifndef OBJECT_H
#define OBJECT_H

#include "graphics.h"
#include <stdbool.h>
#include "unit.h"

#define OBJ_SIZE BLOCK_SIZE

typedef enum obj_type
{
	OBJ_TYPE_NULL = -1,
	OBJ_TYPE_HERO = 0,
	OBJ_TYPE_GROUND = 1,
	OBJ_TYPE_BLOCK = 2,
	OBJ_TYPE_BALL = 3,
	OBJ_TYPE_DOOR = 4,
	OBJ_TYPE_PRESENT = 5,
	OBJ_TYPE_BALLOONS = 6
} obj_type;

#define OBJ_NUM_O_TYPES ( OBJ_TYPE_BALLOONS + 1 )

typedef struct object
{
	graphics gfx;
	int x;
	int y;
	obj_type type;
	bool on_ground;
	dir_x dir;
} object;

void obj_run( void );
void obj_render( void );

#endif