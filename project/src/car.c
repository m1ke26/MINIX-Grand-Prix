#include "car.h"
#include "track.h"
#include "camera.h"
#include <stdlib.h>
#include <math.h>

car_t* create_car(double x, double y, double speed, double angle, xpm_map_t xpms[]) {
  car_t *car = (car_t *) malloc(sizeof(car_t));
  if (car == NULL) return NULL;

  car->x = x;
  car->y = y;
  car->speed = speed;
  car->angle = angle;
  
  for (int i = 0; i < 16; i++) {
    car->sprites[i] = create_sprite(xpms[i]);
  }

  return car;
}

bool move_car(car_t *car) {
  if (car == NULL) return false;

  double radians = car->angle * (M_PI / 180.0);

  // Calculate new position
  double new_x = car->x + car->speed * sin(radians);
  double new_y = car->y - car->speed * cos(radians);

  // Get index to reference sprite width/height
  int idx = (int)((car->angle + 11.25) / 22.5) % 16;
  int w = car->sprites[idx]->width;
  int h = car->sprites[idx]->height;

  // Map border clamping (track collision handles the rest)
  if (new_x < 0) { new_x = 0; car->speed = 0; }
  if (new_x > 1600 - w) { new_x = 1600 - w; car->speed = 0; }
  if (new_y < 0) { new_y = 0; car->speed = 0; }
  if (new_y > 1200 - h) { new_y = 1200 - h; car->speed = 0; }

  // Track collision: check surface at new position
  surface_t surface = track_car_surface((int)new_x, (int)new_y, w, h);

  if (surface == SURFACE_BLOCKED) {
    // Try sliding along X axis only
    surface_t sx = track_car_surface((int)new_x, (int)car->y, w, h);
    if (sx != SURFACE_BLOCKED) {
      car->x = new_x;
      if (sx == SURFACE_SLOW) car->speed *= 0.95;
    }
    // Try sliding along Y axis only
    else {
      surface_t sy = track_car_surface((int)car->x, (int)new_y, w, h);
      if (sy != SURFACE_BLOCKED) {
        car->y = new_y;
        if (sy == SURFACE_SLOW) car->speed *= 0.95;
      }
      // Can't move at all - stop
      else {
        car->speed = 0;
      }
    }
  } else {
    car->x = new_x;
    car->y = new_y;
    // Slow zone: reduce speed gradually
    if (surface == SURFACE_SLOW) car->speed *= 0.95;
  }

  return true;
}

void destroy_car(car_t *car) {
  if (car == NULL) return;
  for (int i = 0; i < 16; i++) {
    if (car->sprites[i] != NULL) {
      destroy_sprite(car->sprites[i]);
    }
  }
  free(car);
}

void draw_car(car_t *car) {
  if (car == NULL) return;
  // Calculate which sprite to use based on angle (0 to 7)
  // Angle: 0 = Up, 45 = Up-Right, 90 = Right, etc.
  int idx = (int)((car->angle + 11.25) / 22.5) % 16;
  // Draw relative to camera position
  int screen_x = (int)car->x - camera_get_x();
  int screen_y = (int)car->y - camera_get_y();
  sprite_draw(car->sprites[idx], screen_x, screen_y);
}

static void update_speed(car_t *car, bool key_w, bool key_s) {
  if (key_w) {
    car->speed += CAR_ACCEL; 
    if (car->speed > CAR_MAX_SPEED) car->speed = CAR_MAX_SPEED; 
  } else if (key_s) {
    car->speed -= CAR_BRAKE; 
    if (car->speed < CAR_MAX_REV_SPEED) car->speed = CAR_MAX_REV_SPEED; 
  } else {
    // Natural deceleration / drag when keys are released
    if (car->speed > CAR_FRICTION) car->speed -= CAR_FRICTION;
    else if (car->speed < -CAR_FRICTION) car->speed += CAR_FRICTION;
    else car->speed = 0;
  }
}

static void update_angle(car_t *car, bool key_a, bool key_d) {
  if (car->speed == 0.0) return;

  double direction = (car->speed > 0) ? 1.0 : -1.0;
  double speed_ratio = fabs(car->speed) / CAR_MAX_SPEED;
  
  if (speed_ratio < CAR_MIN_TURN_RATIO) {
    speed_ratio = CAR_MIN_TURN_RATIO; 
  }

  double turn_rate = CAR_TURN_RATE * direction * speed_ratio;

  if (key_a) car->angle -= turn_rate;
  if (key_d) car->angle += turn_rate;

  // Normalize angle between 0 and 360 degrees
  if (car->angle < 0) car->angle += 360.0;
  if (car->angle >= 360.0) car->angle -= 360.0;
}

void update_car_physics(car_t *car, bool key_w, bool key_s, bool key_a, bool key_d) {
  if (car == NULL) return;

  update_speed(car, key_w, key_s);
  update_angle(car, key_a, key_d);
  move_car(car);
}
