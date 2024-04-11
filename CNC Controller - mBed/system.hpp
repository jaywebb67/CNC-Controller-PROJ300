#ifndef _SYSTEM_HPP_
#define _SYSTEM_HPP_

#include "mbed.h"
#include "steppers.hpp"

#define max(a,b) (((a) > (b)) ? (a) : (b))
#define min(a,b) (((a) < (b)) ? (a) : (b))

// Bit field and masking macros
#define bit(n) (1 << n) 
#define bit_true(x,mask) (x) |= (mask)
#define bit_false(x,mask) (x) &= ~(mask)
#define bit_istrue(x,mask) ((x & mask) != 0)
#define bit_isfalse(x,mask) ((x & mask) == 0)

#define clear_vector_float(a) memset(a, 0.0, sizeof(float)*N_AXIS)

#define EXEC_STATUS_REPORT  bit(0) // bitmask 00000001
#define EXEC_CYCLE_START    bit(1) // bitmask 00000010
#define EXEC_CYCLE_STOP     bit(2) // bitmask 00000100
#define EXEC_FEED_HOLD      bit(3) // bitmask 00001000
#define EXEC_RESET          bit(4) // bitmask 00010000
#define EXEC_SAFETY_DOOR    bit(5) // bitmask 00100000
#define EXEC_MOTION_CANCEL  bit(6) // bitmask 01000000

// Alarm executor bit map.
// NOTE: EXEC_CRITICAL_EVENT is an optional flag that must be set with an alarm flag. When enabled,
// this halts Grbl into an infinite loop until the user aknowledges the problem and issues a soft-
// reset command. For example, a hard limit event needs this type of halt and aknowledgement.
#define EXEC_CRITICAL_EVENT     bit(0) // bitmask 00000001 (SPECIAL FLAG. See NOTE:)
#define EXEC_ALARM_HARD_LIMIT   bit(1) // bitmask 00000010
#define EXEC_ALARM_SOFT_LIMIT   bit(2) // bitmask 00000100
#define EXEC_ALARM_ABORT_CYCLE  bit(3) // bitmask 00001000
#define EXEC_ALARM_PROBE_FAIL   bit(4) // bitmask 00010000
#define EXEC_ALARM_HOMING_FAIL  bit(5) // bitmask 00100000

// Define system state bit map. The state variable primarily tracks the individual functions
// of Grbl to manage each without overlapping. It is also used as a messaging flag for
// critical events.
#define STATE_IDLE          0      // Must be zero. No flags.
#define STATE_ALARM         bit(0) // In alarm state. Locks out all g-code processes. Allows settings access.
#define STATE_CHECK_MODE    bit(1) // G-code check mode. Locks out planner and motion only.
#define STATE_HOMING        bit(2) // Performing homing cycle
#define STATE_CYCLE         bit(3) // Cycle is running or motions are being executed.
#define STATE_HOLD          bit(4) // Active feed hold
#define STATE_SAFETY_DOOR   bit(5) // Safety door is ajar. Feed holds and de-energizes system.
#define STATE_MOTION_CANCEL bit(6) // Motion cancel by feed hold and return to idle. 

// Define system suspend states.
#define SUSPEND_DISABLE       0      // Must be zero.
#define SUSPEND_ENABLE_HOLD   bit(0) // Enabled. Indicates the cycle is active and currently undergoing a hold.
#define SUSPEND_ENABLE_READY  bit(1) // Ready to resume with a cycle start command.
#define SUSPEND_ENERGIZE      bit(2) // Re-energizes output before resume.
#define SUSPEND_MOTION_CANCEL bit(3) // Cancels resume motion. Used by probing routine.

#define HOMING_CYCLE_0 (1<<Z_AXIS)                // REQUIRED: First move Z to clear workspace.
#define HOMING_CYCLE_1 ((1<<X_AXIS)|(1<<Y_AXIS))  // OPTIONAL: Then move X,Y at the same time.

extern volatile uint8_t sys_probe_state;   // Probing state value.  Used to coordinate the probing cycle with stepper ISR.
extern volatile uint8_t sys_rt_exec_state;  // Global realtime executor bitflag variable for state management. See EXEC bitmasks.
extern volatile uint8_t sys_rt_exec_alarm;  // Global realtime executor bitflag variable for setting various alarms.

// Define global system variables
typedef struct {
  uint8_t abort;                 // System abort flag. Forces exit back to main loop for reset.
  uint8_t state;                 // Tracks the current state of Grbl.
  uint8_t suspend;               // System suspend bitflag variable that manages holds, cancels, and safety door.
  uint8_t soft_limit;            // Tracks soft limit errors for the state machine. (boolean)
  
  int32_t position[N_AXIS];      // Real-time machine (aka home) position vector in steps. 
                                 // NOTE: This may need to be a volatile variable, if problems arise.                             

  int32_t probe_position[N_AXIS]; // Last probe position in machine coordinates and steps.
  uint8_t probe_succeeded;        // Tracks if last probing cycle was successful.
  uint8_t homing_axis_lock;       // Locks axes when limits engage. Used as an axis motion mask in the stepper ISR.
} system_t;

extern system_t sys;

// Returns machine position of axis 'idx'. Must be sent a 'step' array.
float system_convert_axis_steps_to_mpos(int32_t *steps, uint8_t idx);

// Updates a machine 'position' array based on the 'step' array sent.
void system_convert_array_steps_to_mpos(float *position, int32_t *steps);


#endif