#include "car.h"
#include <stdlib.h>
#include <math.h>

car_t *create_car(double x, double y, double speed, double angle, xpm_map_t xpms[])
{
  car_t *car = (car_t *)malloc(sizeof(car_t));
  if (car == NULL)
    return NULL;

  car->x = x;
  car->y = y;
  car->speed = speed;
  car->angle = angle;

  for (int i = 0; i < 8; i++)
  {
    car->sprites[i] = create_sprite(xpms[i]);
  }

  return car;
}

bool move_car(car_t *car, track_t *track)
{
  if (car == NULL)
    return false;

  double radians = car->angle * (M_PI / 180.0);

  double old_x = car->x;
  double old_y = car->y;
  // Update position based on speed and angle
  car->x += car->speed * sin(radians);
  car->y -= car->speed * cos(radians); // Subtract because y increases downwards

  int terrain = collision_track(track, (int)car->x + 50, (int)car->y + 50);

  if (terrain == 1)
  {
    car->x = old_x;
    car->y = old_y;
    car->speed = 0;
  }

  else if (terrain == 2)
  {
    car->speed *= 0.90;
  }

  // Get index to reference sprite width/height
  int idx = (int)((car->angle + 22.5) / 45.0) % 8;

  // Border collision handling (after position update)
  if (car->x < 0)
  {
    car->x = 0;
    car->speed = 0; // Stop car on crash
  }
  if (car->x > 2956 - car->sprites[idx]->width)
  {
    car->x = 2956 - car->sprites[idx]->width;
    car->speed = 0;
  }
  if (car->y < 0)
  {
    car->y = 0;
    car->speed = 0;
  }
  if (car->y > 2217 - car->sprites[idx]->height)
  {
    car->y = 2217 - car->sprites[idx]->height;
    car->speed = 0;
  }

  return true;
}

void destroy_car(car_t *car)
{
  if (car == NULL)
    return;
  for (int i = 0; i < 8; i++)
  {
    if (car->sprites[i] != NULL)
    {
      destroy_sprite(car->sprites[i]);
    }
  }
  free(car);
}

void draw_car(car_t *car, int cam_x, int cam_y)
{
  if (car == NULL)
    return;
  // Calculate which sprite to use based on angle (0 to 7)
  // Angle: 0 = Up, 45 = Up-Right, 90 = Right, etc.
  int idx = (int)((car->angle + 22.5) / 45.0) % 8;
  sprite_draw(car->sprites[idx], (int)car->x - cam_x, (int)car->y - cam_y);
}

static void update_speed(car_t *car, bool key_w, bool key_s)
{
  if (key_w)
  {
    car->speed += CAR_ACCEL;
    if (car->speed > CAR_MAX_SPEED)
      car->speed = CAR_MAX_SPEED;
  }
  else if (key_s)
  {
    car->speed -= CAR_BRAKE;
    if (car->speed < CAR_MAX_REV_SPEED)
      car->speed = CAR_MAX_REV_SPEED;
  }
  else
  {
    // Natural deceleration / drag when keys are released
    if (car->speed > CAR_FRICTION)
      car->speed -= CAR_FRICTION;
    else if (car->speed < -CAR_FRICTION)
      car->speed += CAR_FRICTION;
    else
      car->speed = 0;
  }
}

static void update_angle(car_t *car, bool key_a, bool key_d)
{
  if (car->speed == 0.0)
    return;

  double direction = (car->speed > 0) ? 1.0 : -1.0;
  double speed_ratio = fabs(car->speed) / CAR_MAX_SPEED;

  if (speed_ratio < CAR_MIN_TURN_RATIO)
  {
    speed_ratio = CAR_MIN_TURN_RATIO;
  }

  double turn_rate = CAR_TURN_RATE * direction * speed_ratio;

  if (key_a)
    car->angle -= turn_rate;
  if (key_d)
    car->angle += turn_rate;

  // Normalize angle between 0 and 360 degrees
  if (car->angle < 0)
    car->angle += 360.0;
  if (car->angle >= 360.0)
    car->angle -= 360.0;
}

void update_car_physics(car_t *car, bool key_w, bool key_s, bool key_a, bool key_d, track_t *track)
{
  if (car == NULL)
    return;

  update_speed(car, key_w, key_s);
  update_angle(car, key_a, key_d);
  move_car(car, track);
}
