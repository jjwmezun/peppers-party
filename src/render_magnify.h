#ifndef RENDER_MAGNIFY_H
#define RENDER_MAGNIFY_H

#include <SDL2/SDL.h>

#define WINDOW_TYPE SDL_WINDOW_OPENGL

int  render_magnify( int n );
void render_adjust_magnification( void );

#endif