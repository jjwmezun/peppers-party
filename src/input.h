#ifndef INPUT_H
#define INPUT_H

#include <stdbool.h>
#include <SDL2/SDL.h>

typedef enum inputs
{
	INPUT_LEFT,
	INPUT_RIGHT,
	INPUT_JUMP,
	INPUT_PICKUP
} inputs;

void input_send_presses( SDL_Keycode key );
void input_send_releases( SDL_Keycode key );
void input_update( void );
bool input_pressed( inputs in );
bool input_held( inputs in );

#endif