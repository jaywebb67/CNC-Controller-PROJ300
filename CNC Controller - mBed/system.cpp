#include "system.hpp"

volatile uint8_t sys_probe_state;   // Probing state value.  Used to coordinate the probing cycle with stepper ISR.
volatile uint8_t sys_rt_exec_state;  // Global realtime executor bitflag variable for state management. See EXEC bitmasks.
volatile uint8_t sys_rt_exec_alarm;  // Global realtime executor bitflag variable for setting various alarms.


system_t sys;

// Returns machine position of axis 'idx'. Must be sent a 'step' array.
float system_convert_axis_steps_to_mpos(int32_t *steps, uint8_t idx){
    return 1;
}

// Updates a machine 'position' array based on the 'step' array sent.
void system_convert_array_steps_to_mpos(float *position, int32_t *steps){
    
}