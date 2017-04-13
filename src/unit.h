#ifndef UNIT_H
#define UNIT_H

#define BLOCK_SIZE 32
#define WINDOW_WIDTH_BLOCKS 20
#define WINDOW_HEIGHT_BLOCKS 11
#define WINDOW_WIDTH_PIXELS ( WINDOW_WIDTH_BLOCKS * BLOCK_SIZE )
#define WINDOW_HEIGHT_PIXELS ( WINDOW_HEIGHT_BLOCKS * BLOCK_SIZE )
#define BLOCKS_TO_PIXELS( n ) ( (n) * BLOCK_SIZE )

typedef enum dir
{
	DIR_LEFT,
	DIR_RIGHT,
	DIR_UP,
	DIR_DOWN
} dir;

typedef enum dir_x
{
	DIRX_LEFT,
	DIRX_RIGHT
} dir_x;

typedef enum dir_y
{
	DIRY_UP,
	DIRY_DOWN
} dir_y;

#endif