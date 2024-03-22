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

#define stepperNumber 3

#define Xstep_HIGH   GPIOB->BSRR = (1U << 15);
#define Xstep_LOW  GPIOB->BSRR = (1U << (15+16));

#define Ystep_HIGH   GPIOB->BSRR = (1U << 8);
#define Ystep_LOW  GPIOB->BSRR = (1U << (8+16));

#define Zstep_HIGH   GPIOB->BSRR = (1U << 5);
#define Zstep_LOW  GPIOB->BSRR = (1U << (5+16));

#define max_velocity 10000
#define max_acceleration 1000
#define mm_per_revoltuion 4
#define steps_per_revoltuion 1600

struct stepperInfo {
  // externally defined parameters
  float acceleration;
  volatile unsigned long minStepInterval; // ie. max speed, smaller is faster
  void (*dirFunc)(int);
  void (*stepFunc)();

  // derived parameters
  unsigned int c0;                // step interval for first step, determines acceleration
  long stepPosition;              // current position of stepper (total of all movements taken so far)

  // per movement variables (only changed once per movement)
  volatile int dir;                        // current direction of movement, used to keep track of position
  volatile unsigned int totalSteps;        // number of steps requested for current movement
  volatile bool movementDone = false;      // true if the current movement has been completed (used by main program to wait for completion)
  volatile unsigned int rampUpStepCount;   // number of steps taken to reach either max speed, or half-way to the goal (will be zero until this number is known)
  volatile unsigned long estStepsToSpeed;  // estimated steps required to reach max speed
  volatile unsigned long estTimeForMove;   // estimated time (interrupt ticks) required to complete movement
  volatile unsigned long rampUpStepTime;
  volatile float speedScale;               // used to slow down this motor to make coordinated movement with other motors

  // per iteration variables (potentially changed every interrupt)
  volatile unsigned int n;                 // index in acceleration curve, used to calculate next interval
  volatile float d;                        // current interval length
  volatile unsigned long accel_di[2000];               // above variable truncated
  volatile unsigned long decel_di[2000];               // above variable truncated
  volatile unsigned int stepCount;         // number of steps completed in current movement
};

extern volatile stepperInfo steppers[stepperNumber];

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

void stepperInit(int accel, int max_speed);
void disableStepperInterrupt(int axis);
void runAndWait();
void prepareMovement(int whichMotor, long steps);

// void move_axis_stepper_motor(int axis, uint32_t steps,int dir);
// void oneRevolution(uint16_t microstepping,int axis,int dir);
// void stepper_direction(int axis,int dir);
// void coordinatedMotion(int32_t steps, uint16_t target_velocity);


#endif