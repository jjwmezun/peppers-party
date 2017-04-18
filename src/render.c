#include <assert.h>
#include "objects.h"
#include "render.h"
#include "render_magnify.h"
#include <SDL2/SDL_image.h>
#include "unit.h"


// Constants
#define RENDER_GFX_FILENAME "/home/jjwmezun/Documents/c/peppers-party/resources/gfx.png"


// Private Members
SDL_Window *window_;
SDL_Renderer *renderer_;
SDL_Texture *render_gfx_;
SDL_Rect render_surface_ = { 0, 0, WINDOW_WIDTH_PIXELS, WINDOW_HEIGHT_PIXELS };
SDL_Color render_bg_color_ = { 255, 0, 255, 255 };
SDL_Rect render_src_  = { 0, 0, OBJ_SIZE, OBJ_SIZE };
SDL_Rect render_dest_ = { 0, 0, OBJ_SIZE, OBJ_SIZE };


// Demo Victory
#define RENDER_DEMO_WIN_SCREEN_FILENAME "/home/jjwmezun/Documents/c/peppers-party/resources/demo-win-screen.png"
SDL_Texture *render_demo_win_screen_;
bool render_win_started_ = false;


// Private Function Declarations
bool render_load_gfx( void );
bool render_load_texture( const char *filename );
void render_rect( const SDL_Rect *coords, const SDL_Color *color );


// Function Implementations
bool render_init( void )
{
	//render_adjust_magnification();
	render_dest_.w = render_dest_.h = render_magnify( OBJ_SIZE );
	render_surface_.w = render_magnify( WINDOW_WIDTH_PIXELS );
	render_surface_.h = render_magnify( WINDOW_HEIGHT_PIXELS );

	window_ = SDL_CreateWindow
	( 
		"Pepper's Party",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		render_magnify( WINDOW_WIDTH_PIXELS ),
		render_magnify( WINDOW_HEIGHT_PIXELS ),
		WINDOW_TYPE
	);
	
	if ( window_ == NULL )
	{
		SDL_Log( "Unable to create SDL_Window: %s", SDL_GetError() );
		return false;
	}
	
	renderer_ = SDL_CreateRenderer( window_, -1, 0 );
	if ( renderer_ == NULL )
	{
		SDL_Log( "Unable to create SDL_Renderer: %s", SDL_GetError() );
		render_destroy();
		return false;
	}
	
	if ( IMG_Init( IMG_INIT_PNG ) == 0 )
	{
		SDL_Log( "Unable to load SDL_Image: %s", IMG_GetError() );
		render_destroy();
		return false;
	}
	
	if ( !render_load_gfx() )
	{
		return false;
	}
	
	return true;
}

void render_destroy( void )
{
	SDL_DestroyTexture( render_gfx_ );
	IMG_Quit();
	SDL_DestroyRenderer( renderer_ );
	SDL_DestroyWindow( window_ );
}

bool render_load_gfx( void )
{
	return render_load_texture( RENDER_GFX_FILENAME );
}

bool render_load_texture( const char *filename )
{
	if ( render_gfx_ != NULL )
	{
		SDL_DestroyTexture( render_gfx_ );
	}

	SDL_Surface *surface = IMG_Load( filename );

	if ( surface == NULL )
	{
		SDL_Log( "Unable to load surface \"%s\": %s", filename, SDL_GetError() );
		return false;
	}

	render_gfx_ = SDL_CreateTextureFromSurface( renderer_, surface );

	if ( render_gfx_ == NULL )
	{
		SDL_Log( "Unable to create texture for \"%s\": %s", filename, SDL_GetError() );
		return false;
	}

	SDL_FreeSurface( surface );
	return true;
}

void render_start()
{
	SDL_SetRenderDrawColor( renderer_, 0, 0, 0, 0 );
	SDL_RenderClear( renderer_ );
	render_rect( &render_surface_, &render_bg_color_ );
}

void render_end()
{
	SDL_RenderPresent( renderer_ );
}

#define render_flip( gfx ) ( ( gfx->flip_x ) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE )

void render_obj( const graphics *gfx )
{
	render_src_.x = gfx->src_x;
	render_src_.y = gfx->src_y;
	render_dest_.x = render_magnify( ( int )gfx->dest_x );
	render_dest_.y = render_magnify( ( int )gfx->dest_y );
	
	SDL_RenderCopyEx( renderer_, render_gfx_, &render_src_, &render_dest_, gfx->rotation, NULL, render_flip( gfx ) );
}

void render_rect( const SDL_Rect *coords, const SDL_Color *color )
{
	SDL_SetRenderDrawColor( renderer_, color->r, color->g, color->b, color->a );
	SDL_RenderFillRect( renderer_, coords );
}

void render_demo_win_screen( void )
{
	if ( !render_win_started_ )
	{
		render_load_texture( RENDER_DEMO_WIN_SCREEN_FILENAME );
	}
	
	const SDL_Rect src = { 0, 0, WINDOW_WIDTH_PIXELS, WINDOW_HEIGHT_PIXELS };
	SDL_Rect dest = src;
	dest.x = render_magnify( dest.x );
	dest.y = render_magnify( dest.y );
	SDL_RenderCopy( renderer_, render_gfx_, &src, &dest );
}