#include "input.h"
#include "mezun_tools.h"
#include "objects.h"
#include "render.h"


// Private Constants
#define OBJ_BOTTOM( obj ) ( (obj.y) + OBJ_SIZE )
#define OBJ_RIGHT( obj )  ( (obj.x) + OBJ_SIZE )
#define OBJ_MAP_SIZE ( WINDOW_WIDTH_BLOCKS * WINDOW_HEIGHT_BLOCKS )
#define NUM_O_SOLIDS 4
#define OBJ_GFX_NULL ( ( graphics ) { OBJ_SIZE, 0, 0, 0, 0.0, false } )
#define OBJ_NULL { ( graphics ) { OBJ_SIZE, 0, 0, 0, 0.0, false }, 0, 0, OBJ_TYPE_BLOCK, false, DIRX_LEFT }
#define obj_can_go_left( obj )  ( obj.dir == DIRX_LEFT && !obj_is_solid_left( &obj ) && obj.x != 0 )
#define obj_can_go_right( obj ) ( obj.dir == DIRX_RIGHT && !obj_is_solid_right( &obj ) && obj.x != WINDOW_WIDTH_BLOCKS - 1 )


// Private Members
object obj_hero_ =
{
	( graphics ){ 0, OBJ_SIZE, 0, 0, 0.0, false },
	0,
	0,
	OBJ_TYPE_HERO,
	false,
	DIRX_RIGHT
};

const obj_type obj_solids_[ NUM_O_SOLIDS ] = { OBJ_TYPE_GROUND, OBJ_TYPE_BLOCK, OBJ_TYPE_BALL, OBJ_TYPE_BALLOONS };

const int obj_map_[ OBJ_MAP_SIZE ] =
{
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 4, 0, 0, 0, 0, 0, 3, 0, 0, 2, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 6, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 1, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 1, 1
};

object obj_list_[ OBJ_MAP_SIZE ] = { OBJ_NULL };
bool obj_started_ = false;


// Private Function Declarations
void obj_init( void );
void obj_all_update( object *obj );

void obj_map_iter( void ( *f )( int n ) );
void obj_blocks_render( void );
void obj_blocks_render_func( int n );
void obj_set_func( int n );
void obj_set( void );
graphics obj_gfx_from_type( obj_type type, int x, int y );

void obj_hero_update( void );
void obj_hero_render( void );
void obj_hero_jump( void );

obj_type obj_type_below( const object *obj );
obj_type obj_type_left( const object *obj );
obj_type obj_type_right( const object *obj );
bool obj_is_solid( obj_type type );
bool obj_is_solid_left( const object *obj );
bool obj_is_solid_right( const object *obj );
bool obj_is_solid_below( const object *obj );
//void obj_print_map_func( int n );
//void obj_print_map( void );


// Public Function Implementations
void obj_run( void )
{
	if ( !obj_started_ )
	{
		obj_init();
	}
	
	obj_hero_update();
	obj_all_update( &obj_hero_ );
}

void obj_render( void )
{
	obj_blocks_render();
	obj_hero_render();
}


// Private Function Implementations
void obj_all_update( object *obj )
{
	if ( obj->gfx.dest_x < BLOCKS_TO_PIXELS( obj->x ) )
	{
		obj->gfx.dest_x += 2;
	}
	else if ( obj->gfx.dest_x > BLOCKS_TO_PIXELS( obj->x ) )
	{
		obj->gfx.dest_x -= 2;
	}
	
	if ( obj->y >= WINDOW_HEIGHT_BLOCKS - 1 )
	{
		obj->on_ground = true;
		obj->y = WINDOW_HEIGHT_BLOCKS - 1;
	}
	else if ( obj_is_solid( obj_type_below( &obj_hero_ ) ) )
	{
		obj->on_ground = true;
	}
	else
	{
		obj->on_ground = false;
	}
	
	if ( obj->gfx.dest_y < BLOCKS_TO_PIXELS( obj->y ) && obj->gfx.dest_x == BLOCKS_TO_PIXELS( obj->x ) )
	{
		obj->gfx.dest_y += 4;
	}
	else if ( obj->gfx.dest_y > BLOCKS_TO_PIXELS( obj->y ) )
	{
		obj->gfx.dest_y -= 4;
	}
	else if ( !obj->on_ground )
	{
		++obj->y;
	}
}

void obj_hero_update( void )
{
	if ( obj_hero_.gfx.dest_y == BLOCKS_TO_PIXELS( obj_hero_.y ) && obj_hero_.gfx.dest_x == BLOCKS_TO_PIXELS( obj_hero_.x ) )
	{
		if ( input_held( INPUT_RIGHT ) )
		{
			if ( obj_can_go_right( obj_hero_ ) )
			{
				++obj_hero_.x;
			}
			
			obj_hero_.dir = DIRX_RIGHT;
		}
		else if ( input_held( INPUT_LEFT ) )
		{
			if ( obj_can_go_left( obj_hero_ ) )
			{
				--obj_hero_.x;
			}
			
			obj_hero_.dir = DIRX_LEFT;
		}
	}
	
	if ( obj_hero_.on_ground && obj_hero_.gfx.dest_y == BLOCKS_TO_PIXELS( obj_hero_.y ) && obj_hero_.gfx.dest_x == BLOCKS_TO_PIXELS( obj_hero_.x ) )
	{
		if ( input_held( INPUT_JUMP ) )
		{
			obj_hero_jump();
		}
	}
}

void obj_hero_render( void )
{
	if ( obj_hero_.dir == DIRX_RIGHT )
	{
		obj_hero_.gfx.flip_x = true;
	}
	else
	{
		obj_hero_.gfx.flip_x = false;
	}
	
	render_obj( &obj_hero_.gfx );
}

void obj_init( void )
{
	obj_set();
	obj_started_ = true;
}

void obj_map_iter( void ( *f )( int n ) )
{
	for ( int i = 0; i < OBJ_MAP_SIZE; ++i )
	{
		f( i );
	}
}

void obj_blocks_render( void )
{
	obj_map_iter( obj_blocks_render_func );
}

void obj_blocks_render_func( int n )
{
	render_obj( &obj_list_[ n ].gfx );
}

graphics obj_gfx_from_type( obj_type type, int x, int y )
{
	int sx = ( int )type;
	
	if ( type > 0 && type < OBJ_NUM_O_TYPES )
	{
		return ( graphics ) { BLOCKS_TO_PIXELS( --sx ), 0, BLOCKS_TO_PIXELS( x ), BLOCKS_TO_PIXELS( y ), 0.0, false };
	}
	else
	{
		return OBJ_GFX_NULL;
	}
}

void obj_set_func( int n )
{
	object *o = &obj_list_[ n ];
	o->type = ( obj_type )obj_map_[ n ];
	
	if ( o->type != OBJ_TYPE_NULL )
	{
		o->gfx = obj_gfx_from_type( o->type, xofn( n, WINDOW_WIDTH_BLOCKS ), yofn( n, WINDOW_WIDTH_BLOCKS ) );
	}
}

void obj_set( void )
{
	obj_map_iter( obj_set_func );
}

obj_type obj_type_below( const object *obj )
{
	const int other_n = nofxy( obj->x, ( obj->y + 1 ), WINDOW_WIDTH_BLOCKS );
	return obj_list_[ other_n ].type;
}

obj_type obj_type_right( const object *obj )
{
	const int other_n = nofxy( ( obj->x + 1 ), obj->y, WINDOW_WIDTH_BLOCKS );
	return obj_list_[ other_n ].type;
}

obj_type obj_type_left( const object *obj )
{
	const int other_n = nofxy( ( obj->x - 1 ), obj->y, WINDOW_WIDTH_BLOCKS );
	return obj_list_[ other_n ].type;
}

void obj_hero_jump( void )
{
	--obj_hero_.y;
	
	if ( obj_can_go_right( obj_hero_ ) )
	{
		++obj_hero_.x;
	}
	else if ( obj_can_go_left( obj_hero_ ) )
	{
		--obj_hero_.x;
	}
}

bool obj_is_solid( obj_type type )
{
	for ( int i = 0; i < NUM_O_SOLIDS; ++i )
	{
		if ( obj_solids_[ i ] == type )
		{
			return true;
		}
	}
	
	return false;
}

bool obj_is_solid_right( const object *obj )
{
	return obj_is_solid( obj_type_right( obj ) );
}

bool obj_is_solid_left( const object *obj )
{
	return obj_is_solid( obj_type_left( obj ) );
}

bool obj_is_solid_below( const object *obj )
{
	return obj_is_solid( obj_type_below( obj ) );
}

/* DEBUG
void obj_print_map( void )
{
	printf( "\n" );
	obj_map_iter( obj_print_map_func );
	printf( "\n" );
}

void obj_print_map_func( int n )
{
	printf( "%d, ", obj_map_[ n ] );
	
	if( xofn( n, WINDOW_WIDTH_BLOCKS ) == WINDOW_WIDTH_BLOCKS - 1 )
	{
		printf( "\n" );
	}
}*/