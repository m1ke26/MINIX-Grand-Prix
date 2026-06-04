#ifndef _SPEEDOMETER_H_
#define _SPEEDOMETER_H_

#include <lcom/lcf.h>
#include "sprite.h"
#include "car.h"

#define SPEEDO_X 10
#define SPEEDO_Y 440
#define SPEEDO_W 150
#define SPEEDO_H 150

/* Needle pivot center relative to sprite top-left */
#define NEEDLE_CX 72
#define NEEDLE_CY 75
#define NEEDLE_LEN 35

/* Angle range: -220 deg (speed=0) to 40 deg (speed=max), in degrees */
#define NEEDLE_ANGLE_MIN (-220.0)
#define NEEDLE_ANGLE_MAX (40.0)

void speedometer_init();
void speedometer_draw(float speed);
void speedometer_destroy();

#endif /* _SPEEDOMETER_H_ */
