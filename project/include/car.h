#ifndef _CAR_H_
#define _CAR_H_

#include "track.h"
#include "input.h"
#include "sprite.h"

#define CAR_MAX_SPEED       5.0   // Maximum forward speed
#define CAR_MAX_REV_SPEED  -1.5   // Maximum reverse speed
#define CAR_ACCEL           0.15  // Acceleration rate
#define CAR_BRAKE           0.20  // Braking rate
#define CAR_FRICTION        0.08  // Friction coefficient
#define CAR_TURN_RATE       3.0   // Turn rate
#define CAR_MIN_TURN_RATIO  0.3   // Minimum turn ratio
#define CAR_MAX_SPEED_BOOST 7.0   // Maximum speed boost
#define CAR_BOOST_ACCEL     0.30  // Boost acceleration
#define CAR_BOOST_MAX       100.0 // Maximum boost amount
#define CAR_BOOST_DRAIN     0.84  // Boost drain rate
#define CAR_BOOST_RECHARGE  0.34  // Boost recharge rate
#define GRIP_HIGH           0.85  // Normal grip level
#define GRIP_LOW            0.08  // Low grip level
#define GRIP_ENGAGE_RATE    0.35  // how fast grip drops when drifting
#define GRIP_RECOVER_RATE   0.06  // how fast grip returns
#define DRIFT_MIN_SPEED     2.0   // min speed to initiate drift


typedef struct {
    double x, y;
    double speed;
    double angle; /* degrees, 0 = up */
    double velocity_angle;
    double grip;
    bool is_drifting;
    double boost_amount;
    int num_sprites;
    sprite_t *sprites[48];
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
car_t* create_car(double x, double y, double speed, double angle, xpm_map_t xpms[], int num_sprites);
/**
    @brief Destroys the car and frees any allocated resources.
    @param car Pointer to the car to be destroyed.
**/
void destroy_car(car_t *car);

/**
    @brief Draws the car on the screen.
    @param car Pointer to the car to be drawn.
    @param cam_x Camera x-coordinate.
    @param cam_y Camera y-coordinate.
    @param sprite_idx Index of the sprite to draw.
**/
void draw_car(car_t *car, int cam_x, int cam_y, int sprite_idx);
/**
    @brief Moves the car based on its speed and angle.
    @param car Pointer to the car to be moved.
    @return True if the car was moved successfully, false otherwise.
**/
bool move_car(car_t *car, track_t *track);
/**
    @brief Updates the physics of the car based on the given key presses.
    @param car Pointer to the car to be updated.
    @param input Current gameplay input state.
**/
void update_car_physics(car_t *car, const game_input_t *input, track_t *track);

/**
 * @brief Returns the index of the sprite to use for the car based on its current angle.
 * @param car Pointer to the car.
 * @return Index of the sprite to use for drawing the car.
 */
int car_sprite_index(car_t *car);

#endif /* _CAR_H_ */
