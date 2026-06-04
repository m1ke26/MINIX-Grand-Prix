#ifndef _MINIMAP_H_
#define _MINIMAP_H_

#include <lcom/lcf.h>
#include "sprite.h"
#include "track.h"

#define MINIMAP_X 610
#define MINIMAP_Y 10
#define MINIMAP_W 180
#define MINIMAP_H 135

void minimap_init(int track_index, track_t *track);
void minimap_draw(int car_x, int car_y);
void minimap_destroy();

#endif /* _MINIMAP_H_ */
