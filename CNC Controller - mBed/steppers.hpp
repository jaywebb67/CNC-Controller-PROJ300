#ifndef _STEPPERS_H_
#define _STEPPERS_H_

#include "mbed.h"

#define stepperEN_pin PB_9
#define xdir_pin PB_4
#define xstep_pin PB_15_ALT1
#define ydir_pin PB_3
#define ystep_pin PB_8
#define zdir_pin PB_6
#define zstep_pin PB_5

#define max_velocity 10000
#define max_acceleration 4000

typedef enum {

    acceleration = 1,
    constantV = 2,
    deceleration = 3

} motionPhase_t;

typedef enum {

    ALL = 0,
    x_axis = 1,
    Y_axis = 2,
    Z_axis = 3,


} axis_t;

void stepperInit(float frequency);
void enableSteppers(int axis,bool state);
void move_axis_stepper_motor(int axis, uint32_t steps,int dir);
void oneRevolution(uint16_t microstepping,int axis,int dir);
void stepper_direction(int axis,int dir);
void coordinatedMotion(int32_t steps, uint16_t target_velocity);


#endif