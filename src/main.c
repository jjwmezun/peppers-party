#include "input.h"
#include "mezun_tools.h"
#include "objects.h"
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "render.h"

// Constants
#define FPS  60
#define FPMS ( 1000 / FPS )


// Private Members
int main_frame_count_;
bool running_;


// Private Function Declarations
bool main_init( void );
void main_end( void );
void main_loop( void );
void main_render( void );
void maintain_frame_rate( int loop_start_time );


// Public Functions
int main()
{
	if ( !main_init() )
	{
		return 1;
	}

	while ( running_ )
	{
		main_loop();
	}
	
	main_end();
	
	return 0;
}


// Private Functions
void main_loop( void )
{
	Uint32 loop_start_time = SDL_GetTicks();
	
	SDL_Event e;
	if ( SDL_PollEvent( &e ) )
	{
		switch( e.type )
		{
			case ( SDL_QUIT ):
				running_ = false;
			break;

			case ( SDL_KEYDOWN ):

				if ( SDLK_ESCAPE == e.key.keysym.sym )
				{
					running_ = false;
					break;
				}

				input_send_presses( e.key.keysym.sym );
			break;

			case ( SDL_KEYUP ):
				input_send_releases( e.key.keysym.sym );
			break;
		}
	}

	obj_run();
	main_render();
	input_update();
	
	++main_frame_count_;
	maintain_frame_rate( loop_start_time );
}

bool main_init( void )
{
	if ( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_EVENTS ) != 0 )
	{
		SDL_Log( "Unable to initialize SDL: %s", SDL_GetError() );
		return false;
	}
	
	if ( !render_init() )
	{
		SDL_Log( "Render initialization failure: %s", SDL_GetError() );
		SDL_Quit();
		return false;
	}

	mezun_rand_init();
	running_ = true;
	
	return true;
}

void main_end( void )
{
	render_destroy();
	SDL_Quit();
}

void main_render( void )
{
	render_start();
	obj_render();
	render_end();
}

void maintain_frame_rate( int loop_start_time )
{
	if ( FPMS > SDL_GetTicks() - loop_start_time )
	{
		SDL_Delay( FPMS - ( SDL_GetTicks() - loop_start_time ) );
	}
}