#ifndef MEZUN_TOOLS_H
#define MEZUN_TOOLS_H

#include <float.h>
#include <stdlib.h>
#include <time.h>

#define MIN( a, b ) ( ( (a) < (b) ) ? (a) : (b) )
#define MAX( a, b ) ( ( (a) > (b) ) ? (a) : (b) )

#define dbequal( a, b ) ( fabs( (a) - (b) ) < DBL_MIN )
#define dbzero( a ) ( dbequal( a, 0 ) )

#define mezun_rand_init() ( srand( ( unsigned int )time( NULL ) ) )
#define mezun_rand_int( max, min ) ( rand() % ( MAX( (max), (min) ) + 1 - MIN( (min), (max) ) ) + MIN( (min), (max) ) )

#define xofn( n, w ) ( (n) % (w) )
#define yofn( n, w ) ( ( int )floor( ( double )( n ) / ( double )( w ) ) )
#define nofxy( x, y, w ) ( ( (y) * w ) + x )

#define print_int( n ) ( printf( "%d\n", (n) ) )

#endif