#ifndef _CAR_H_
#define _CAR_H_

#include "sprite.h"

#define CAR_MAX_SPEED       5.0
#define CAR_MAX_REV_SPEED  -2.5
#define CAR_ACCEL           0.15
#define CAR_BRAKE           0.20
#define CAR_FRICTION        0.08
#define CAR_TURN_RATE       3.5
#define CAR_MIN_TURN_RATIO  0.3
#define CAR_MAX_SPEED_BOOST 7.0
#define CAR_BOOST_ACCEL     0.30
#define CAR_BOOST_MAX       100.0
#define CAR_BOOST_DRAIN     0.84
#define CAR_BOOST_RECHARGE  0.34


typedef struct {
    double x, y;
    double speed;
    double angle; /* degrees, 0 = up */
    double boost_amount;
    sprite_t *sprites[16];
} car_t;

/**
    @brief Creates a new car with the given parameters.
    @param x The x-coordinate of the car.
    @param y The y-coordinate of the car.
    @param speed The speed of the car.
    @param angle The angle of the car.
    @param xpms Array of xpm maps for the car sprites.
    @return Pointer to the new car.
**/    
car_t* create_car(double x, double y, double speed, double angle, xpm_map_t xpms[]);
/**
    @brief Destroys the car and frees any allocated resources.
    @param car Pointer to the car to be destroyed.
**/    
void destroy_car(car_t *car);

/**
    @brief Draws the car on the screen.
    @param car Pointer to the car to be drawn.
**/    
void draw_car(car_t *car);
/**
    @brief Moves the car based on its speed and angle.
    @param car Pointer to the car to be moved.
    @return True if the car was moved successfully, false otherwise.
**/    
bool move_car(car_t *car);
/**
    @brief Updates the physics of the car based on the given key presses.
    @param car Pointer to the car to be updated.
    @param key_w Boolean indicating if the W key is pressed.
    @param key_s Boolean indicating if the S key is pressed.
    @param key_a Boolean indicating if the A key is pressed.
    @param key_d Boolean indicating if the D key is pressed.
    @param key_space Boolean indicating if the Space key is pressed.
**/    
void update_car_physics(car_t *car, bool key_w, bool key_s, bool key_a, bool key_d, bool key_space);


/** @brief Draws the boost indicator for the car.
    @param car Pointer to the car for which to draw the indicator.
**/
void draw_boost_indicator(car_t *car);

#endif /* _CAR_H_ */
