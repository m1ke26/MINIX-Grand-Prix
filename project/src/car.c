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
  car->boost_amount = CAR_BOOST_MAX;
  car->velocity_angle = angle;
  car->grip = GRIP_HIGH;
  car->is_drifting = false;
  
  for (int i = 0; i < 16; i++) {
    car->sprites[i] = create_sprite(xpms[i]);
  }

  return car;
}

bool move_car(car_t *car) {
  if (car == NULL) return false;

 // Blend velocity_angle toward car->angle based on grip
  double angle_diff = car->angle - car->velocity_angle;
  while (angle_diff > 180.0) angle_diff -= 360.0;
  while (angle_diff < -180.0) angle_diff += 360.0;
  car->velocity_angle += angle_diff * car->grip;

  // Normalize velocity_angle
  if (car->velocity_angle < 0) car->velocity_angle += 360.0;
  if (car->velocity_angle >= 360.0) car->velocity_angle -= 360.0;

  // Use velocity_angle for actual movement (not car->angle)
  double radians = car->velocity_angle * (M_PI / 180.0);

  double new_x = car->x + car->speed * sin(radians);
  double new_y = car->y - car->speed * cos(radians);

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

static bool update_boost(car_t *car, bool boost_pressed) {
  bool boost_active = boost_pressed && car->boost_amount > 0.0;

  if (boost_active) {
    car->boost_amount -= CAR_BOOST_DRAIN;
    if (car->boost_amount < 0.0) car->boost_amount = 0.0;
  } else if (car->boost_amount < CAR_BOOST_MAX) {
    car->boost_amount += CAR_BOOST_RECHARGE;
    if (car->boost_amount > CAR_BOOST_MAX) car->boost_amount = CAR_BOOST_MAX;
  }

  return boost_active;
}

static void update_speed(car_t *car, bool accelerate, bool brake, bool boost_active) {
  if (accelerate) {
    double max_speed = boost_active ? CAR_MAX_SPEED_BOOST : CAR_MAX_SPEED;
    double accel = boost_active ? CAR_BOOST_ACCEL : CAR_ACCEL;

    if (car->speed < max_speed) car->speed += accel;
    else car->speed -= CAR_FRICTION;

    if (car->speed > max_speed) car->speed = max_speed;
  } else if (brake) {
    car->speed -= CAR_BRAKE; 
    if (car->speed < CAR_MAX_REV_SPEED) car->speed = CAR_MAX_REV_SPEED; 
  } else {
    // Natural deceleration / drag when keys are released
    if (car->speed > CAR_FRICTION) car->speed -= CAR_FRICTION;
    else if (car->speed < -CAR_FRICTION) car->speed += CAR_FRICTION;
    else car->speed = 0;
  }
}

static void update_angle(car_t *car, bool turn_left, bool turn_right) {
  if (car->speed == 0.0) return;

  double direction = (car->speed > 0) ? 1.0 : -1.0;
  double speed_ratio = fabs(car->speed) / CAR_MAX_SPEED;
  
  if (speed_ratio < CAR_MIN_TURN_RATIO) {
    speed_ratio = CAR_MIN_TURN_RATIO; 
  }

  // Turn faster when drifting so angle pulls away from velocity_angle
  double drift_multiplier = car->is_drifting ? 1.6 : 1.0;
  double turn_rate = CAR_TURN_RATE * direction * speed_ratio * drift_multiplier;

  if (turn_left) car->angle -= turn_rate;
  if (turn_right) car->angle += turn_rate;

  // Normalize angle between 0 and 360 degrees
  if (car->angle < 0) car->angle += 360.0;
  if (car->angle >= 360.0) car->angle -= 360.0;
}

static void update_drift(car_t *car, bool handbrake) {
  car->is_drifting = handbrake && fabs(car->speed) > DRIFT_MIN_SPEED;

  if (car->is_drifting) {
    car->grip = car->grip + (GRIP_LOW - car->grip) * GRIP_ENGAGE_RATE;
  } else {
    car->grip = car->grip + (GRIP_HIGH - car->grip) * GRIP_RECOVER_RATE;
  }
}

void update_car_physics(car_t *car, const game_input_t *input) {
  if (car == NULL || input == NULL) return;

  bool boost_active = update_boost(car, input->boost);
  update_speed(car, input->accelerate, input->brake, boost_active);
  update_angle(car, input->turn_left, input->turn_right);
  move_car(car);
  update_drift(car, input->handbrake);
}
