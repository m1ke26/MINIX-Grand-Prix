#include "car.h"
#include "track.h"
#include "camera.h"
#include "video-card.h"
#include <stdlib.h>
#include <math.h>

car_t* create_car(double x, double y, double speed, double angle, xpm_map_t xpms[]) {
  car_t *car = (car_t *) malloc(sizeof(car_t));
  if (car == NULL) return NULL;

  car->x = x;
  car->y = y;
  car->speed = speed;
  car->angle = angle;
  car->boost_amount = CAR_BOOST_MAX;
  
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

static bool update_boost(car_t *car, bool key_space) {
  bool boost_active = key_space && car->boost_amount > 0.0;

  if (boost_active) {
    car->boost_amount -= CAR_BOOST_DRAIN;
    if (car->boost_amount < 0.0) car->boost_amount = 0.0;
  } else if (car->boost_amount < CAR_BOOST_MAX) {
    car->boost_amount += CAR_BOOST_RECHARGE;
    if (car->boost_amount > CAR_BOOST_MAX) car->boost_amount = CAR_BOOST_MAX;
  }

  return boost_active;
}

static void update_speed(car_t *car, bool key_w, bool key_s, bool boost_active) {
  if (key_w) {
    double max_speed = boost_active ? CAR_MAX_SPEED_BOOST : CAR_MAX_SPEED;
    double accel = boost_active ? CAR_BOOST_ACCEL : CAR_ACCEL;

    if (car->speed < max_speed) car->speed += accel;
    else car->speed -= CAR_FRICTION;

    if (car->speed > max_speed) car->speed = max_speed;
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

void update_car_physics(car_t *car, bool key_w, bool key_s, bool key_a, bool key_d, bool key_space) {
  if (car == NULL) return;

  bool boost_active = update_boost(car, key_space);
  update_speed(car, key_w, key_s, boost_active);
  update_angle(car, key_a, key_d);
  move_car(car);
}

void draw_boost_indicator(car_t *car) {
  if (car == NULL) return;

  int bar_width = 140;
  int bar_height = 14;
  int x = 800 - bar_width - 20;
  int y = 600 - bar_height - 20;
  double boost_ratio = car->boost_amount / CAR_BOOST_MAX;
  int fill_width = (int)(bar_width * boost_ratio);

  vg_buf_draw_rect(x - 2, y - 2, bar_width + 4, bar_height + 4, 0x000000);
  vg_buf_draw_rect(x, y, bar_width, bar_height, 0x1A1A1A);
  vg_buf_draw_rect(x, y, fill_width, bar_height, 0x007BFF);
}
