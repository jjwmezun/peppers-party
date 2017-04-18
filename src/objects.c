#include <assert.h>
#include "input.h"
#include "mezun_tools.h"
#include "objects.h"
#include "render.h"


// Private Constants
#define OBJ_BOTTOM( obj ) ( (obj.y) + OBJ_SIZE )
#define OBJ_RIGHT( obj )  ( (obj.x) + OBJ_SIZE )
#define OBJ_MAP_SIZE ( WINDOW_WIDTH_BLOCKS * WINDOW_HEIGHT_BLOCKS )
#define NUM_O_SOLIDS 4
#define NUM_O_MOVEABLES 2
#define obj_can_go_left( obj )  ( obj.dir == DIRX_LEFT && !obj_is_solid_left( &obj ) && obj.x != 0 )
#define obj_can_go_right( obj ) ( obj.dir == DIRX_RIGHT && !obj_is_solid_right( &obj ) && obj.x != WINDOW_WIDTH_BLOCKS - 1 )
#define OBJ_NULL_ENTRY -1
#define obj_gfx_aligned( obj ) ( obj.gfx.dest_x == BLOCKS_TO_PIXELS( obj.x ) && obj.gfx.dest_y == BLOCKS_TO_PIXELS( obj.y ) )
#define OBJ_HERO_TURN_LOCK_LIMIT 12
#define OBJ_BALLS_ROLL_MAX 5


// Private Members
object obj_hero_;
int obj_hero_turn_lock_ = 0;
bool obj_hero_warp_lock_ = false;
int obj_hero_item_pickup_ = OBJ_NULL_ENTRY;
int obj_balloons_triggered_ = OBJ_NULL_ENTRY;
int obj_balls_rolling_[ OBJ_BALLS_ROLL_MAX ] = { OBJ_NULL_ENTRY, OBJ_NULL_ENTRY, OBJ_NULL_ENTRY, OBJ_NULL_ENTRY, OBJ_NULL_ENTRY };
bool obj_won_ = false;

const obj_type obj_solids_[ NUM_O_SOLIDS ] = { OBJ_TYPE_GROUND, OBJ_TYPE_BLOCK, OBJ_TYPE_BALL, OBJ_TYPE_BALLOONS };
const obj_type obj_moveables_[ NUM_O_MOVEABLES ] = { OBJ_TYPE_BLOCK, OBJ_TYPE_BALL };

const int obj_map_[ OBJ_MAP_SIZE ] =
{
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 2, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 4, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 6, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 1, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 1, 1
};

object obj_list_[ OBJ_MAP_SIZE ];
int num_o_obj = 0;
bool obj_started_ = false;


// Private Function Declarations
void obj_init( void );
void obj_all_movement( object *obj );

void obj_map_iter( void ( *f )( int n ) );
void obj_blocks_render( bool priority );
void obj_set( void );
graphics obj_gfx_from_type( obj_type type, int x, int y );
void obj_destroy( int n );

void obj_hero_update( void );
void obj_hero_render( void );
void obj_hero_jump( void );
bool obj_hero_can_move_left( void );
bool obj_hero_can_move_right( void );
void obj_hero_victory( void );

int obj_is_there_type_in_pos( int x, int y, obj_type type );
int obj_find_n_by_pos( int x, int y );
const object *obj_find_by_pos( int x, int y );
int obj_find_n_left( const object *obj );
int obj_find_n_right( const object *obj );
obj_type obj_type_by_pos( int x, int y );
obj_type obj_type_above( const object *obj );
obj_type obj_type_below( const object *obj );
obj_type obj_type_left( const object *obj );
obj_type obj_type_right( const object *obj );
bool obj_is_solid( obj_type type );
bool obj_is_solid_left( const object *obj );
bool obj_is_solid_right( const object *obj );
bool obj_is_solid_above( const object *obj );
bool obj_is_solid_below( const object *obj );

void obj_warp_hero_to_next_door( object *obj );
void obj_roll_ball( int n, dir_x direction );
void obj_update_rolling_balls( void );
void obj_stop_ball_rolling( int n );

void obj_print_map_func( int n );
void obj_print_map( void );


// Public Function Implementations
void obj_run( void )
{
	if ( !obj_started_ )
	{
		obj_init();
	}
	
	if ( !obj_won_ )
	{
		obj_hero_update();
		obj_all_movement( &obj_hero_ );

		obj_update_rolling_balls();
		for ( int i = 0; i < OBJ_MAP_SIZE && obj_list_[ i ].type != OBJ_TYPE_NULL; ++i )
		{
			object* o = &obj_list_[ i ];
			for ( int t = 0; t < NUM_O_MOVEABLES; ++t )
			{
				if ( obj_moveables_[ t ] == o->type )
				{
					obj_all_movement( o );
					break;
				}
			}
		}

		if ( obj_hero_item_pickup_ != OBJ_NULL_ENTRY )
		{
			obj_list_[ obj_hero_item_pickup_ ].y = obj_hero_.y - 1;
			obj_list_[ obj_hero_item_pickup_ ].x = obj_hero_.x;
		}
	}
}

void obj_render( void )
{
	if ( obj_won_ )
	{
		render_demo_win_screen();
	}
	else
	{
		obj_blocks_render( false );
		obj_hero_render();
		obj_blocks_render( true );
	}
}


// Private Function Implementations
void obj_all_movement( object *obj )
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
	else if ( obj_is_solid( obj_type_below( obj ) ) )
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
	if ( obj_gfx_aligned( obj_hero_ ) )
	{
		obj_type block_hero_is_on = obj_type_by_pos( obj_hero_.x, obj_hero_.y );
		
		if ( OBJ_TYPE_PRESENT == block_hero_is_on )
		{
			obj_hero_victory();
		}
		else if ( OBJ_TYPE_DOOR == block_hero_is_on )
		{
			if ( !obj_hero_warp_lock_ )
			{
				obj_warp_hero_to_next_door( &obj_hero_ );
				obj_hero_warp_lock_ = true;
				return;
			}
		}
		else
		{
			obj_hero_warp_lock_ = false;
		}

		if ( obj_balloons_triggered_ != OBJ_NULL_ENTRY )
		{
			if ( obj_hero_.x != obj_list_[ obj_balloons_triggered_ ].x )
			{
				obj_destroy( obj_balloons_triggered_ );
				obj_balloons_triggered_ = OBJ_NULL_ENTRY;
			}
		}
		else
		{
			int block_below = obj_find_n_by_pos( obj_hero_.x, obj_hero_.y + 1 );
			if ( OBJ_NULL_ENTRY != block_below && OBJ_TYPE_BALLOONS == obj_list_[ block_below ].type )
			{
				obj_balloons_triggered_ = block_below;
			}
		}

		if ( input_held( INPUT_RIGHT ) )
		{
			if ( obj_can_go_right( obj_hero_ ) && obj_hero_turn_lock_ == 0 )
			{
				if ( OBJ_NULL_ENTRY != obj_hero_item_pickup_ && obj_is_solid_right( &obj_list_[ obj_hero_item_pickup_ ] ) )
				{
					obj_hero_item_pickup_ = OBJ_NULL_ENTRY;
				}

				++obj_hero_.x;
			}
			else if ( obj_hero_.dir != DIRX_RIGHT )
			{
				obj_hero_turn_lock_ = OBJ_HERO_TURN_LOCK_LIMIT;
			}
			else if ( OBJ_TYPE_BALL == obj_type_right( &obj_hero_ ) )
			{
				obj_roll_ball( obj_find_n_by_pos( obj_hero_.x + 1, obj_hero_.y ), DIRX_RIGHT );
			}
			
			obj_hero_.dir = DIRX_RIGHT;
		}
		else if ( input_held( INPUT_LEFT ) )
		{
			if ( obj_can_go_left( obj_hero_ ) && obj_hero_turn_lock_ == 0 )
			{
				if ( OBJ_NULL_ENTRY != obj_hero_item_pickup_ && obj_is_solid_left( &obj_list_[ obj_hero_item_pickup_ ] ) )
				{
					obj_hero_item_pickup_ = OBJ_NULL_ENTRY;
				}
				
				--obj_hero_.x;
			}
			else if ( obj_hero_.dir != DIRX_LEFT )
			{
				obj_hero_turn_lock_ = OBJ_HERO_TURN_LOCK_LIMIT;
			}
			else if ( OBJ_TYPE_BALL == obj_type_left( &obj_hero_ ) )
			{
				obj_roll_ball( obj_find_n_by_pos( obj_hero_.x - 1, obj_hero_.y ), DIRX_LEFT );
			}
			
			obj_hero_.dir = DIRX_LEFT;
		}
	}
	
	if ( obj_hero_turn_lock_ > 0 )
	{
		--obj_hero_turn_lock_;
	}
	
	if ( obj_hero_.on_ground && obj_gfx_aligned( obj_hero_ ) )
	{
		if ( input_held( INPUT_JUMP ) )
		{
			obj_hero_jump();
		}
		
		if ( input_held( INPUT_PICKUP ) )
		{
			if ( OBJ_NULL_ENTRY == obj_hero_item_pickup_ )
			{
				if ( !obj_is_solid_above( &obj_hero_ ) )
				{
					switch ( obj_hero_.dir )
					{
						case ( DIRX_LEFT ):
						{
							const int item_num = obj_is_there_type_in_pos( obj_hero_.x - 1, obj_hero_.y, OBJ_TYPE_BLOCK );

							if ( item_num != OBJ_NULL_ENTRY )
							{
								if ( obj_gfx_aligned( obj_list_[ item_num ] ) )
								{
									if ( !obj_is_solid_above( &obj_list_[ item_num ] ) )
									{
										obj_hero_item_pickup_ = item_num;
									}
								}
							}
						}
						break;

						case ( DIRX_RIGHT ):
						{
							const int item_num = obj_is_there_type_in_pos( obj_hero_.x + 1, obj_hero_.y, OBJ_TYPE_BLOCK );

							if ( item_num != OBJ_NULL_ENTRY )
							{
								if ( obj_gfx_aligned( obj_list_[ item_num ] ) )
								{
									if ( !obj_is_solid_above( &obj_list_[ item_num ] ) )
									{
										obj_hero_item_pickup_ = item_num;
									}
								}
							}
						}
						break;
					}
				}
			}
			else
			{
				if ( obj_gfx_aligned( obj_list_[ obj_hero_item_pickup_ ] ) )
				{
					switch ( obj_hero_.dir )
					{
						case ( DIRX_LEFT ):
							if ( OBJ_TYPE_NULL == obj_type_left( &obj_list_[ obj_hero_item_pickup_ ] ) )
							{
								obj_list_[ obj_hero_item_pickup_ ].x = obj_hero_.x - 1;
								obj_hero_item_pickup_ = OBJ_NULL_ENTRY;
							}
						break;

						case ( DIRX_RIGHT ):
							if ( OBJ_TYPE_NULL == obj_type_right( &obj_list_[ obj_hero_item_pickup_ ] ) )
							{
								obj_list_[ obj_hero_item_pickup_ ].x = obj_hero_.x + 1;
								obj_hero_item_pickup_ = OBJ_NULL_ENTRY;
							}
						break;
					}
				}
			}
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

bool obj_hero_can_move_left( void )
{
	bool cond = obj_can_go_left( obj_hero_ );
	
	if ( obj_hero_item_pickup_ != OBJ_NULL_ENTRY )
	{
		cond = cond && !obj_is_solid_left( &obj_list_[ obj_hero_item_pickup_ ] );
	}
	
	return cond;
}

bool obj_hero_can_move_right( void )
{	
	bool cond = obj_can_go_right( obj_hero_ );
	
	if ( obj_hero_item_pickup_ != OBJ_NULL_ENTRY )
	{
		cond = cond && !obj_is_solid_right( &obj_list_[ obj_hero_item_pickup_ ] );
	}
	
	return cond;
}

void obj_init( void )
{
	obj_hero_ = ( object )
	{
		( graphics ){ 0, OBJ_SIZE, 0, 0, 0.0, false, false },
		0,
		0,
		OBJ_TYPE_HERO,
		false,
		DIRX_RIGHT
	};
	
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

void obj_blocks_render( bool priority )
{
	for ( int i = 0; i < OBJ_MAP_SIZE && obj_list_[ i ].type != OBJ_TYPE_NULL; ++i )
	{
		if ( priority == obj_list_[ i ].gfx.priority )
		{
			render_obj( &obj_list_[ i ].gfx );
		}
	}
}

graphics obj_gfx_from_type( obj_type type, int x, int y )
{
	const bool priority = OBJ_TYPE_BLOCK == type || OBJ_TYPE_BALL == type;
	const int sx = ( int )type;
	return ( graphics ) { BLOCKS_TO_PIXELS( sx - 1 ), 0, BLOCKS_TO_PIXELS( x ), BLOCKS_TO_PIXELS( y ), 0.0, false, priority };
}

void obj_set( void )
{
	for ( int n = 0; n < OBJ_MAP_SIZE; ++n )
	{
		const obj_type type = ( obj_type )obj_map_[ n ];
		
		if ( type != OBJ_TYPE_NULL )
		{
			object *o = &obj_list_[ num_o_obj ];
			o->type = ( obj_type )obj_map_[ n ];
			o->x = xofn( n, WINDOW_WIDTH_BLOCKS );
			o->y = yofn( n, WINDOW_WIDTH_BLOCKS );
			o->gfx = obj_gfx_from_type( o->type, o->x, o->y );
			++num_o_obj;
		}
	}
	
	// Ensure Null is after all items; act as terminator so iterators know when to optimally stop.
	if ( num_o_obj < OBJ_MAP_SIZE )
	{
		obj_list_[ num_o_obj ].type = OBJ_TYPE_NULL;
	}
}

int obj_is_there_type_in_pos( int x, int y, obj_type type )
{
	for ( int i = 0; i < OBJ_MAP_SIZE; ++i )
	{	
		const object* o = &obj_list_[ i ];
		if ( x == o->x && y == o->y && type == o->type )
		{
			return i;
		}
	}

	return OBJ_NULL_ENTRY;
}

int obj_find_n_by_pos( int x, int y )
{
	for ( int i = 0; i < OBJ_MAP_SIZE; ++i )
	{	
		const object* o = &obj_list_[ i ];

		if ( x == o->x && y == o->y )
		{
			return i;
		}
	}

	return OBJ_NULL_ENTRY;
}

const object *obj_find_by_pos( int x, int y )
{
	const int n = obj_find_n_by_pos( x, y );
	
	if ( OBJ_NULL_ENTRY == n )
	{
		return NULL;
	}
	else
	{
		return &obj_list_[ n ];
	}
}

obj_type obj_type_by_pos( int x, int y )
{
	const object *o = obj_find_by_pos( x, y );

	if ( o != NULL )
	{
		return o->type;
	}
	else
	{
		return OBJ_TYPE_NULL;
	}
}

int obj_find_n_left( const object *obj )
{
	return obj_find_n_by_pos( obj->x - 1, obj->y );
}

int obj_find_n_right( const object *obj )
{
	return obj_find_n_by_pos( obj->x + 1, obj->y );
}

obj_type obj_type_above( const object *obj )
{
	return obj_type_by_pos( obj->x, obj->y - 1 );
}

obj_type obj_type_below( const object *obj )
{
	return obj_type_by_pos( obj->x, obj->y + 1 );
}

obj_type obj_type_right( const object *obj )
{
	return obj_type_by_pos( obj->x + 1, obj->y );
}

obj_type obj_type_left( const object *obj )
{
	return obj_type_by_pos( obj->x - 1, obj->y );
}

void obj_hero_jump( void )
{
	bool cond;
	
	if ( obj_hero_item_pickup_ != OBJ_NULL_ENTRY )
	{
		cond = !obj_is_solid_above( &obj_list_[ obj_hero_item_pickup_ ] );
	}
	else
	{
		cond = !obj_is_solid_above( &obj_hero_ );
	}
	
	if ( cond )
	{
		--obj_hero_.y;

		if ( obj_hero_can_move_right() )
		{
			++obj_hero_.x;
		}
		else if ( obj_hero_can_move_left() )
		{
			--obj_hero_.x;
		}
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

bool obj_is_solid_above( const object *obj )
{
	return obj_is_solid( obj_type_above( obj ) );
}

bool obj_is_solid_below( const object *obj )
{
	return obj_is_solid( obj_type_below( obj ) );
}

void obj_warp_hero_to_next_door( object *hero )
{
	for ( int i = 0; i < OBJ_MAP_SIZE && obj_list_[ i ].type != OBJ_TYPE_NULL; ++i )
	{
		const object *o = &obj_list_[ i ];
		
		if ( OBJ_TYPE_DOOR == o->type )
		{
			if ( o->x != hero->x || o->y != hero->y )
			{
				hero->x = o->x;
				hero->y = o->y;
				
				// Make sure the hero warps there immediately.
				hero->gfx.dest_x = o->gfx.dest_x;
				hero->gfx.dest_y = o->gfx.dest_y;
				
				if ( OBJ_NULL_ENTRY != obj_hero_item_pickup_ )
				{
					object *pkup = &obj_list_[ obj_hero_item_pickup_ ];
					
					pkup->x = o->x;
					pkup->y = o->y - 1;
					pkup->gfx.dest_x = o->gfx.dest_x;
					pkup->gfx.dest_y = o->gfx.dest_y;
				}
				
				return;
			}
		}
	}
	
	puts( "ERROR: Only 1 door in map. Can't warp." );
}


void obj_roll_ball( int n, dir_x direction )
{
	int num_o_balls_rolling = 0;
	
	for ( ; num_o_balls_rolling < OBJ_BALLS_ROLL_MAX; ++num_o_balls_rolling )
	{
		// Ensure game doesn't add already-rolling ball 'gain.
		if ( n == obj_balls_rolling_[ num_o_balls_rolling ] )
		{
			return;
		}
		else if ( OBJ_NULL_ENTRY == obj_balls_rolling_[ num_o_balls_rolling ] )
		{
			obj_balls_rolling_[ num_o_balls_rolling ] = n;
			obj_list_[ obj_balls_rolling_[ num_o_balls_rolling ] ].dir = direction;
			return;
		}
	}
	
	puts( "ERROR: Too many balls rolling @ once." );
}

void obj_update_rolling_balls( void )
{
	int num_o_balls_rolling = 0;
	
	for ( ; num_o_balls_rolling < OBJ_BALLS_ROLL_MAX; ++num_o_balls_rolling )
	{
		const int obj_id = obj_balls_rolling_[ num_o_balls_rolling ];
		if ( OBJ_NULL_ENTRY == obj_balls_rolling_[ num_o_balls_rolling ] )
		{
			return;
		}
		else
		{
			if ( obj_gfx_aligned( obj_list_[ obj_id ] ) )
			{
				switch ( obj_list_[ obj_id ].dir )
				{
					case ( DIRX_LEFT ):
						if ( obj_can_go_left( obj_list_[ obj_id ] ) )
						{
							--obj_list_[ obj_id ].x;
						}
						else
						{
							obj_stop_ball_rolling( num_o_balls_rolling );
							return;
						}
					break;

					case ( DIRX_RIGHT ):
						if ( obj_can_go_right( obj_list_[ obj_id ] ) )
						{
							++obj_list_[ obj_id ].x;
						}
						else
						{
							obj_stop_ball_rolling( num_o_balls_rolling );
							return;
						}
					break;
				}
			}
		}
	}
}

void obj_stop_ball_rolling( int n )
{
	assert ( n >= 0 && n < OBJ_BALLS_ROLL_MAX );
	
	int num_o_balls_rolling = 0;
	
	for ( ; num_o_balls_rolling < OBJ_BALLS_ROLL_MAX; ++num_o_balls_rolling )
	{
		if ( OBJ_NULL_ENTRY == obj_balls_rolling_[ num_o_balls_rolling ] )
		{
			assert( num_o_balls_rolling > 0 );
			
			if ( n == num_o_balls_rolling - 1 )
			{
				obj_balls_rolling_[ num_o_balls_rolling - 1 ] = OBJ_NULL_ENTRY;
				return;
			}
			else
			{
				obj_balls_rolling_[ n ] = obj_balls_rolling_[ num_o_balls_rolling - 1 ];
				obj_balls_rolling_[ num_o_balls_rolling - 1 ] = OBJ_NULL_ENTRY;
				return;
			}
		}
	}
}

void obj_hero_victory( void )
{
	obj_won_ = true;
}

void obj_destroy( int n )
{
	assert( n >= 0 && n < num_o_obj );
	
	obj_list_[ n ] = obj_list_[ num_o_obj - 1 ];
	--num_o_obj;
}

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
}