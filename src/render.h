#ifndef RENDERING_H
#define RENDERING_H

#include "graphics.h"
#include <stdbool.h>
#include <SDL2/SDL.h>

bool render_init( void );
void render_destroy( void );

void render_start( void );
void render_end( void );
void render_obj( const graphics *gfx );

#endif