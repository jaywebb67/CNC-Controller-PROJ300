#ifndef _DEFAULTS_HPP_
#define _DEFAULTS_HPP_


// Grbl generic default settings. Should work across different machines.
#define DEFAULT_X_STEPS_PER_MM 400.0
#define DEFAULT_Y_STEPS_PER_MM 400.0
#define DEFAULT_Z_STEPS_PER_MM 400.0
#define DEFAULT_X_MAX_RATE 250.0 // mm/min
#define DEFAULT_Y_MAX_RATE 250.0 // mm/min
#define DEFAULT_Z_MAX_RATE 250.0 // mm/min
#define DEFAULT_X_ACCELERATION 1000
#define DEFAULT_Y_ACCELERATION 1000
#define DEFAULT_Z_ACCELERATION 1000
#define DEFAULT_X_MAX_TRAVEL 300.0 // mm
#define DEFAULT_Y_MAX_TRAVEL 180.0 // mm
#define DEFAULT_Z_MAX_TRAVEL 54.0 // mm
#define DEFAULT_STEP_PULSE_MICROSECONDS 10
#define DEFAULT_STEPPING_INVERT_MASK 0
#define DEFAULT_DIRECTION_INVERT_MASK 0
#define DEFAULT_STEPPER_IDLE_LOCK_TIME 25 // msec (0-254, 255 keeps steppers enabled)
#define DEFAULT_JUNCTION_DEVIATION 0.01 // mm
#define DEFAULT_ARC_TOLERANCE 0.002 // mm
#define DEFAULT_REPORT_INCHES 0 // false
#define DEFAULT_INVERT_ST_ENABLE 0 // false
#define DEFAULT_INVERT_LIMIT_PINS 0 // false
#define DEFAULT_SOFT_LIMIT_ENABLE 1 // false
#define DEFAULT_HARD_LIMIT_ENABLE 1  // false
#define DEFAULT_HOMING_ENABLE 1 // false
#define DEFAULT_HOMING_DIR_MASK 0 // move positive dir
#define DEFAULT_HOMING_FEED_RATE 25.0 // mm/min
#define DEFAULT_HOMING_SEEK_RATE 500.0 // mm/min
#define DEFAULT_HOMING_DEBOUNCE_DELAY 250 // msec (0-65k)
#define DEFAULT_HOMING_PULLOFF 1.0 // mm


#endif