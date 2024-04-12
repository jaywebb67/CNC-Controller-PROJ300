#include "system.hpp"
#include "settings.hpp"
#include "Gcode.hpp"

system_t sys;

//   float steps_per_mm[N_AXIS];
//   float max_rate[N_AXIS];
//   float acceleration[N_AXIS];
//   float max_travel[N_AXIS];

//   // Remaining Grbl settings
//   uint8_t pulse_microseconds;
//   uint8_t step_invert_mask;
//   uint8_t dir_invert_mask;
//   uint8_t stepper_idle_lock_time; // If max value 255, steppers do not disable.
//   float junction_deviation;
//   float arc_tolerance;
  
//   uint8_t flags;  // Contains default boolean settings

//   uint8_t homing_dir_mask;
//   float homing_feed_rate;
//   float homing_seek_rate;
//   uint16_t homing_debounce_delay;
//   float homing_pulloff;

void reportSettings(){

    printf("$0=%d\r\n",settings.pulse_microseconds);
    printf("$1=%d\r\n",settings.stepper_idle_lock_time);   
    printf("$2=%d\r\n",settings.step_invert_mask);
    printf("$3=%d\r\n",settings.dir_invert_mask);    
    printf("$4=%d\r\n",(bit_istrue(settings.flags,BITFLAG_INVERT_ST_ENABLE)));
    printf("$5=%d\r\n",(bit_istrue(settings.flags,BITFLAG_INVERT_LIMIT_PINS)));
    printf("$6=%d\r\n",(bit_istrue(settings.flags,BITFLAG_INVERT_PROBE_PIN))); 
    printf("$11=%f\r\n",settings.junction_deviation);
    printf("$12=%f\r\n",settings.arc_tolerance);
    printf("$13=%d\r\n",(bit_istrue(settings.flags,BITFLAG_REPORT_INCHES)));
    printf("$20=%d\r\n",(bit_istrue(settings.flags,BITFLAG_SOFT_LIMIT_ENABLE)));
    printf("$21=%d\r\n",(bit_istrue(settings.flags,BITFLAG_HARD_LIMIT_ENABLE)));
    printf("$22=%d\r\n",(bit_istrue(settings.flags,BITFLAG_HOMING_ENABLE)));
    printf("$23=%d\r\n",(int)settings.homing_dir_mask);
    printf("$24=%f\r\n",settings.homing_feed_rate);
    printf("$25=%f\r\n",settings.homing_seek_rate);
    printf("$26=%u\r\n",settings.homing_debounce_delay);
    printf("$27=%f\r\n",settings.homing_pulloff);
    uint8_t idx, set_idx;
    uint8_t val = AXIS_SETTINGS_START_VAL;
    for (set_idx=0; set_idx<AXIS_N_SETTINGS; set_idx++) {
        for (idx=0; idx<N_AXIS; idx++) {
            printf("$%d = ",(val+idx));
            switch (set_idx) {
                case 0: 
                    printf("%f\r\n",settings.steps_per_mm[idx]); 
                    break;
                case 1: 
                    printf("%f",settings.max_rate[idx]); 
                    break;
                case 2: 
                    printf("%f",settings.acceleration[idx]); 
                    break;
                case 3: 
                    printf("%f",settings.max_travel[idx]); 
                    break;
            }
        }
        val += AXIS_SETTINGS_INCREMENT;
    }
}
void report_ngc_parameters(){

    float coord_data[N_AXIS];
    uint8_t coord_select, i;
    for (coord_select = 0; coord_select <= SETTING_INDEX_NCOORD; coord_select++) { 
        if (!(settings_read_coord_data(coord_select,coord_data))) { 
            printf("Failed to read coordinate settings\r\n");
            return;
        } 
        printf("[G");
        switch (coord_select) {
            case 6: 
                printf("28"); 
                break;
            case 7: 
                printf("30"); 
                break;
            default: 
                printf("%d",coord_select+54); 
                break; // G54-G59
        }  
        printf(":");         
        for (i=0; i<N_AXIS; i++) {
            printf("%f",coord_data[i]);
            if (i < (N_AXIS-1)) { 
                printf(","); 
            }
            else { 
                printf("]\r\n"); 
            }
        } 
    }

    printf("[G92:"); // Print G92,G92.1 which are not persistent in memory
    for (i=0; i<N_AXIS; i++) {
        printf("%f",line_state.coord_offset[i]);
        if (i < (N_AXIS-1)) { 
            printf(","); 
        }
        else { 
            printf("]\r\n"); 
        }
    } 
    printf("[TLO:%f]\r\n",line_state.tool_length_offset); // Print tool length offset value
}


void reportParserModes(){

    printf("Current modal modes:\r\n");
    switch(line_state.modal.motion){
        case G0: printf("Motion modal: Rapid/Traverse Motion\r\n"); break;
        case G1: printf("Motion modal: Linear Motion\r\n"); break;
        case G2: printf("Motion modal: CW Arc\r\n"); break;
        case G3: printf("Motion modal: CCW Arc\r\n"); break;
        case G38_2: printf("Motion modal: Probe Cycle\r\n"); break;
        default: printf("Motion modal: None\r\n"); break;
    }

    printf("Coord Selection Modal: %d\r\n",(line_state.modal.coordSystem + 54));

    switch(line_state.modal.plane){
        case G17: printf("Plane modal: G17/XY\r\n"); break;
        case G18: printf("Plane modal: G18/ZX\r\n"); break;
        case G19: printf("Plane modal: G19/YZ\r\n"); break;
    }

    if (line_state.modal.units == G21) { 
        printf("Units Modal: G21/MM\r\n"); 
    }
    else { 
        printf("Units Modal: G20/Inches\r\n"); 
    }

    if (line_state.modal.distance == G90) { 
        printf("Distance Modal: G90/Absolute"); 
    }
    else { 
        printf("Distance Modal: G91/Relative"); 
    }

    if (line_state.modal.feed == G93) { 
        printf("Feed Modal: G93/Inverse Rate\r\n"); 
    }
    else { 
        printf("Feed Modal: G94/Units per minute\r\n"); 
    }

    switch (line_state.modal.stopping) {
        case M0: printf("Stopping Modal: M0\r\n"); break;
        case M1: printf("Stopping Modal: M1\r\n"); break;
        case M2: printf("Stopping Modal: M2\r\n"); break;
        default: printf("Stopping Modal: Resume\r\n"); break;
    }

    switch (line_state.modal.spindleTurn) {
        case M3 : printf("Spindle Turn Modal: M3/CW\r\n"); break;
        case M4 : printf("Spindle Turn Modal: M4/CCW\r\n"); break;
        case M5 : printf("Spindle Turn Modal: M5/Disable\r\n"); break;
    }

    switch (line_state.modal.coolant) {
        case M9 : printf("Coolant Modal: M9/Disable\r\n"); break;
        case M8 : printf("Coolant Modal: M8/Flood\r\n"); break;
        case M7 : printf("Coolant Modal: M7/Mist\r\n"); break;
    }

    printf("Tool Number: %d\r\n",line_state.tool);
    printf("Feed Rate: %f\r\n",line_state.feed_rate);
    printf("Spindle Speed: %d\r\n",line_state.spindle_speed);
    printf("Tool Length Offset: %f\r\n",line_state.tool_length_offset);
    printf("X Position: %f",line_state.position[X_AXIS]);
    printf("Y Position: %f",line_state.position[Y_AXIS]);
    printf("Z Position: %f",line_state.position[Z_AXIS]);    
    
}