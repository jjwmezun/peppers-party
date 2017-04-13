#include "render_magnify.h"
#include "unit.h"

#define FORCE_MAGNIFICATION -1

int render_magnification = 1;
int render_left_edge = 0;
int render_top_edge = 0;
SDL_Rect render_top_bar = { 0, 0, 0, 0 };
SDL_Rect render_bottom_bar = { 0, 0, 0, 0 };
SDL_Rect render_left_bar = { 0, 0, 0, 0 };
SDL_Rect render_right_bar = { 0, 0, 0, 0 };
SDL_Rect screen_ = { 0, 0, 0, 0 };

int  render_calc_magnification( int monitor_width, int monitor_height );
void render_adjust_screen( int monitor_width, int monitor_height );
void render_calc_screen_edges( int monitor_width, int monitor_height );
void render_adjust_border_bars( int monitor_width, int monitor_height );

void render_adjust_magnification( void )
{
	int monitor_width = WINDOW_WIDTH_PIXELS;
	int monitor_height = WINDOW_HEIGHT_PIXELS;

	SDL_Rect r;

	if ( FORCE_MAGNIFICATION > -1 )
	{
		render_magnification = FORCE_MAGNIFICATION;
	}
	else if ( SDL_GetDisplayBounds( 0, &r ) != 0 )
	{
		SDL_Log( "SDL_GetDisplayBounds failed: %s", SDL_GetError() );
		render_magnification = 1;
	}
	else
	{
		monitor_width = r.w;
		monitor_height = r.h;

		render_magnification = render_calc_magnification( monitor_width, monitor_height );
	}

	render_adjust_screen( monitor_width, monitor_height );
	render_adjust_border_bars( monitor_width, monitor_height );
}

int render_calc_magnification( int monitor_width, int monitor_height )
{
	double screen_aspect_ratio = (double)WINDOW_WIDTH_PIXELS / (double)WINDOW_HEIGHT_PIXELS;
	double monitor_aspect_ratio = (double)monitor_width / (double)monitor_height;

	if ( monitor_aspect_ratio > screen_aspect_ratio )
	{
		return ( int )floor( monitor_height / (double)WINDOW_HEIGHT_PIXELS );
	}
	else
	{
		return ( int )floor( monitor_width / (double)WINDOW_WIDTH_PIXELS );
	}
}

void render_adjust_screen( int monitor_width, int monitor_height )
{
	render_calc_screen_edges( monitor_width, monitor_height );

	screen_.x = render_left_edge;
	screen_.y = render_top_edge;
	screen_.w = render_magnify( WINDOW_WIDTH_PIXELS );
	screen_.h = render_magnify( WINDOW_HEIGHT_PIXELS );
}

void render_calc_screen_edges( int monitor_width, int monitor_height )
{
	if ( WINDOW_TYPE == SDL_WINDOW_RESIZABLE )
	{
		render_left_edge = 0;
		render_top_edge = 0;
	}
	else
	{
		render_left_edge = ( monitor_width - render_magnify( WINDOW_WIDTH_PIXELS ) ) / 2;
		render_top_edge = ( monitor_height - render_magnify( WINDOW_HEIGHT_PIXELS ) ) / 2;
	}
}

void render_adjust_border_bars( int monitor_width, int monitor_height )
{
	render_top_bar = ( SDL_Rect ){ 0, 0, monitor_width, render_top_edge };
	render_bottom_bar = ( SDL_Rect ){ 0, screen_.y + screen_.h, monitor_width, monitor_height - ( screen_.y + screen_.h ) };
	render_left_bar = ( SDL_Rect ){ 0, 0, render_left_edge, monitor_height };
	render_right_bar = ( SDL_Rect ){ screen_.x + screen_.w, 0, monitor_width - ( screen_.x + screen_.w ), monitor_height };
}

int render_magnify( int n )
{
	return n * render_magnification;
}