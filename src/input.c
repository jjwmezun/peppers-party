// Includes
#include "input.h"


// Defines
#define KEY_LEFT  SDLK_LEFT
#define KEY_RIGHT SDLK_RIGHT
#define KEY_JUMP    SDLK_UP
#define KEY_PICKUP  SDLK_DOWN

#define NUM_O_INPUTS ( INPUT_PICKUP + 1 )


// Members
bool input_pressed_prev_[ NUM_O_INPUTS ] = { false };
bool input_pressed_[ NUM_O_INPUTS ] = { false };
bool input_held_[ NUM_O_INPUTS ]    = { false };


// Function Declarations
bool input_in_range( inputs in );
void input_send_key( SDL_Keycode key, bool type );
void input_register( inputs in, bool type );

// Function Implementations
void input_send_presses( SDL_Keycode key )
{
	input_send_key( key, true );
}

void input_send_releases( SDL_Keycode key )
{
	input_send_key( key, false );
}

void input_send_key( SDL_Keycode key, bool type )
{
	switch ( key )
	{
		case ( KEY_LEFT ):
			input_register( INPUT_LEFT, type );
		break;
			
		case ( KEY_RIGHT ):
			input_register( INPUT_RIGHT, type );
		break;
			
		case ( KEY_JUMP ):
			input_register( INPUT_JUMP, type );
		break;
			
		case ( KEY_PICKUP ):
			input_register( INPUT_PICKUP, type );
		break;
	}
}

void input_register( inputs in, bool type )
{
	input_pressed_[ in ] = type;
	input_held_[ in ] = type;
}

void input_update( void )
{
	for ( int i = 0; i < NUM_O_INPUTS; ++i )
	{
		input_pressed_prev_[ i ] = input_pressed_[ i ];
		input_pressed_[ i ] = false;
	}
}

bool input_pressed( inputs in )
{
	return ( input_in_range( in ) ) ? input_pressed_[ in ] : false;
}

bool input_held( inputs in )
{
	return ( input_in_range( in ) ) ? input_held_[ in ] : false;
}

bool input_in_range( inputs in )
{
	return INPUT_LEFT <= in && in < NUM_O_INPUTS;
}