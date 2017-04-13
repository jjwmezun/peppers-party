#ifndef RECT_H
#define RECT_H

#define rect_half_w( r ) ( floor( r.w / 2 ) )
#define rect_half_h( r ) ( floor( r.h / 2 ) )
#define rect_center_x( r ) ( rect_half_w( r ) + r.x )
#define rect_center_y( r ) ( rect_half_h( r ) + r.y )
#define rect_right( r ) ( r.x + r.w )
#define rect_bottom( r ) ( r.y + r.h )

#define prect_half_w( r ) ( floor( r->w / 2 ) )
#define prect_half_h( r ) ( floor( r->h / 2 ) )
#define prect_center_x( r ) ( prect_half_w( r ) + r->x )
#define prect_center_y( r ) ( prect_half_h( r ) + r->y )
#define prect_right( r ) ( r->x + r->w )
#define prect_bottom( r ) ( r->y + r->h )

#endif