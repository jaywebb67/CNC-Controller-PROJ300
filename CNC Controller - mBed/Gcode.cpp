/*
 * Gcode.c
 *
 *  Created on: Dec 20, 2023
 *      Author: jaywe
*/

#include "Gcode.hpp"
#include <cctype>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <string> 
using namespace std;


lineStruct_block_t line_block;
lineStruct_state_t line_state;

volatile uint8_t axis_word = 0; //XYZ Tracking
volatile uint8_t ijk_words = 0; // IJK tracking 
float h_x2_div_d,x,y = 0;


float parseValue(const char* valueString) {
    // Convert string to floating-point number
    char* endPtr;
    float value = strtof(valueString, &endPtr);

    // Check if conversion was successful
    if (*endPtr != '\0') {
        // Conversion failed, handle error
        printf("Error: Invalid numerical value '%s'\n", valueString);
        // You might want to return a special value indicating an error,
        // or throw an exception depending on your error handling strategy.
    }

    return value;
}

void updateBlockValue(char command, const float value){
    switch (command) {
        case 'R': case 'r':
            line_block.values.radius = value;
            break;
        case 'P': case 'p':
            line_block.values.dwell = value;
            break;
        case 'F': case 'f':
            line_block.values.feed = value;
            break;
        case 'S': case 's':
            line_block.values.spindle = value;
            break;
        case 'T': case 't':
            // Assuming it's an integer value
            line_block.values.toolNo = static_cast<int>(value);
            break;
        case 'X': case 'x':
            line_block.values.xyz[X_AXIS] = value;
            axis_word |= (1 << X_AXIS);
            break;
        case 'Y': case 'y':
            line_block.values.xyz[Y_AXIS] = value;
            axis_word |= (1 << Y_AXIS);
            break;
        case 'Z': case 'z':
            line_block.values.xyz[Z_AXIS] = value;
            axis_word |= (1 << Z_AXIS);
            break;
        case 'I': case 'i':
            line_block.values.ijk[X_AXIS] = value;
            ijk_words |= (1 << X_AXIS);
            break;
        case 'J': case 'j':
            line_block.values.ijk[Y_AXIS] = value;
            ijk_words |= (1 << Y_AXIS);
            break;
        case 'K': case 'k':
            line_block.values.ijk[Z_AXIS] = value;
            ijk_words |= (1 << Z_AXIS);
            break;
        default:
            break;
    }
}

char valueString[20]; // Assuming maximum length of value string
volatile int valueIndex = 0;
volatile float value = 0.0f;
volatile uint8_t axis_0 = 0, axis_1 = 0, axis_linear = 0;
volatile int char_counter = 0;
volatile uint8_t coord_select = 0; // Tracks G10 P coordinate selection for execution
float coordinate_data[N_AXIS] = {0};
float parameter_data[N_AXIS] = {0};
void parse_gcode(char *line) {

    memset(&line_block.nonModalFlag, 0, sizeof(uint16_t)); // Initialize the parser block struct.
    memset(&line_block.modals, 0, sizeof(lineModals_t)); // Initialize the parser block struct.

    line_block.values.xyz[X_AXIS] = 0; line_block.values.xyz[Y_AXIS] = 0; line_block.values.xyz[Z_AXIS] = 0;
    line_block.values.ijk[X_AXIS] = 0; line_block.values.ijk[Y_AXIS] = 0; line_block.values.ijk[Z_AXIS] = 0;

    line_block.values.l = 0; line_block.values.dwell = 0; line_block.values.radius = 0;

    memcpy(&line_block.modals,&line_state.modal,sizeof(lineModals_t)); // Copy current modes
    line_block.values.toolNo = line_state.tool;
    line_block.values.feed = line_state.feed_rate;
    line_block.values.spindle = line_state.spindle_speed;
    line_block.modals.motion = NONE;

    char_counter = 0;
    coord_select = 0;


    axis_word = 0; //XYZ Tracking
    ijk_words = 0; // IJK tracking 


    line_block.nonModalFlag = 0;


    while (line[char_counter] != '\0') {
    	switch(line[char_counter]){
            // if(isalpha(line[char_counter])){
            //     line[char_counter] = (char)toupper(line[char_counter]);
            // }
			case 'G': case 'g':
                char_counter++;
                if(line[char_counter] == ' '){
                    char_counter++;
                }
                switch(line[char_counter]){
                    case '0':
                        line_block.modals.motion = G0;
                        break;
                    case '1':
                        char_counter++;
                        if(line[char_counter] == ' '){
                            line_block.modals.motion = G1;
                            break;
                        }
                        else{
                            switch(line[char_counter]){
                                case '0':
                                    line_block.nonModalFlag |= (1U << G10);
                                    break;
                                case '7':
                                    line_block.modals.plane = G17;
                                    break;
                                case '8':
                                    line_block.modals.plane = G18;
                                    break;
                                case '9':
                                    line_block.modals.plane = G19;
                                    break;
                                default:
                                    break;
                            }
                        }
                        break;
                    case '2':
                        char_counter++;
                        if(line[char_counter] == ' '){
                            line_block.modals.motion = G2;
                            break;
                        }
                        else{
                            switch(line[char_counter]){
                                case '0':
                                    line_block.modals.units = G20;
                                    break;
                                case '1':
                                    line_block.modals.units = G21;
                                    break;
                                case '8':
                                    line_block.nonModalFlag |= ( 1U << G28);
                                    break;
                                default:
                                    break;
                            }
                        }
                        break;
                    case '3':
                        char_counter++;
                        if(line[char_counter] == ' '){
                            line_block.modals.motion = G3;
                        }
                        else{
                            switch(line[char_counter]){
                                case '0':
                                    line_block.nonModalFlag |= ( 1U << G30);
                                    break;
                                case '8':
                                    line_block.modals.motion = G38_2;
                                    break;
                                default:
                                    break;
                            }
                        }
                        break;
                    case '4':
                        char_counter++;
                        if(line[char_counter] == ' '){
                            line_block.nonModalFlag |= ( 1U << G4);
                            break;
                        }
                        else{
                            switch(line[char_counter]){
                                case '0':
                                    line_block.modals.cutterRadius = G40;
                                    break;
                                case '1':
                                    line_block.modals.cutterRadius = G41;
                                    break;
                                case '2':
                                    line_block.modals.cutterRadius = G42;
                                    break;
                                case '3':
                                    line_block.modals.toolLength = G43;
                                    break;
                                case '9':
                                    line_block.modals.toolLength = G49;
                                    break;
                                default:
                                    break;
                            }
                        }  
                        break;                          
                    case '5':
                        char_counter++;
                        switch(line[char_counter]){
                            case '3':
                                line_block.nonModalFlag |= (1U << G53);
                                break;
                            case '4':
                                line_block.modals.coordSystem = G54;
                                break;
                            case '5':
                                line_block.modals.coordSystem = G55;
                                break;
                            case '6':
                                line_block.modals.coordSystem = G56;
                                break;
                            case '7':
                                line_block.modals.coordSystem = G57;
                                break;
                            case '8':
                                line_block.modals.coordSystem = G58;
                                break;
                            case '9':
                                char_counter++;
                                if(line[char_counter] == ' '){
                                    line_block.modals.coordSystem = G59;
                                }
                                else{
                                    char_counter++;
                                    switch (line[char_counter]) {
                                        case '1':
                                            line_block.modals.coordSystem = G59_1;
                                            break;
                                        // case 2:
                                        //     line_block.modals.coordSystem = G59_2;
                                        //     break;
                                        // case 3:
                                        //     line_block.modals.coordSystem = G59_3;
                                        //     break;
                                        default:
                                            break;
                                    }
                            
                                    
                                    break;
                                }
                            default:
                                break;
                        }
                        break;
                    case '6':
                        char_counter++;
                        if (line[char_counter] == 1){
                            char_counter++;
                            if(line[char_counter] == ' '){
                                line_block.modals.pathControl = G61;                                
                            }
                            else{
                                line_block.modals.pathControl = G61_1;
                                char_counter++;
                            }
                        }
                        else if (line[char_counter] == 4) {
                            line_block.modals.pathControl = G64;
                        }
                        break;
                    case '8':
                        char_counter++;
                        // switch (line[char_counter]) {
                        //     case 0:
                        //         line_block.modals.motion = G80;
                        //         break;
                        //     case 1:
                        //         line_block.modals.motion = G81;
                        //         break;
                        //     case 2:
                        //         line_block.modals.motion = G82;
                        //         break;
                        //     case 3:
                        //         line_block.modals.motion = G83;
                        //         break;
                        //     case 4:
                        //         line_block.modals.motion = G84;
                        //         break;
                        //     case 5:
                        //         line_block.modals.motion = G85;
                        //         break;
                        //     case 6:
                        //         line_block.modals.motion = G86;
                        //         break;
                        //     case 7:
                        //         line_block.modals.motion = G87;
                        //         break;
                        //     case 8:
                        //         line_block.modals.motion = G88;
                        //         break;
                        //     case 9:
                        //         line_block.modals.motion = G89;
                        //         break;
                        //     default:
                        //         break;
                        // }
                        break;
                    case '9':
                        char_counter++;
                        switch(line[char_counter]){
                            case '0':
                                line_block.modals.distance = G90;
                                break;
                            case '1':
                                line_block.modals.distance = G91;
                                break;
                            case '3':
                                line_block.modals.feed = G93;
                                break;
                            case '4':
                                line_block.modals.feed = G94;
                                break;
                            case '8':
                                line_block.modals.returnCanned = G98;
                                break;
                            case '9':
                                line_block.modals.returnCanned = G99;
                                break;
                            case '2':
                                char_counter++;
                                if (line[char_counter] == ' '){
                                    line_block.nonModalFlag |= ( 1U << G92);
                                    break;
                                }
                                else{
                                    char_counter++;
                                    switch(line[char_counter]){
                                        case '1':
                                            line_block.nonModalFlag |= ( 1U << G92_1);
                                            break;
                                        // case 2:
                                        //     line_block.nonModalFlag |= ( 1U << G92_2);
                                        //     break;
                                        // case 3:
                                        //     line_block.nonModalFlag |= ( 1U << G92_3);
                                        //     break;
                                        default:
                                            break;
                                    }
                                }
                                break;
                            default:
                                break;
                        }
                        break;
                    default:
                        break;
                }
                break;
			case 'M':case 'm':
                char_counter++;
                if(line[char_counter] == ' '){
                    break;
                }
                else{
                    switch (line[char_counter]) {
                        case '0':
                            line_block.modals.stopping = M0;
                            break;
                        case '1':
                            line_block.modals.stopping = M1;
                            break;
                        case '2':
                            line_block.modals.stopping = M2;
                            break;
                        case '3':
                            char_counter++;
                            if (line[char_counter] == ' '){
                                line_block.modals.spindleTurn = M3;
                                break;
                            }
                            else if (line[char_counter] == 0) {
                                    line_block.modals.stopping = M30;
                                }                            
                            break;
                        case '4':
                            char_counter++;
                            if (line[char_counter] == ' '){
                                line_block.modals.spindleTurn = M4;
                            }
                            else if (line[char_counter] == 8) {
                                line_block.modals.switchOverride = M48;
                            }
                            else if(line[char_counter]){
                                line_block.modals.switchOverride = M49;
                            }
                            break;
                        case '5':
                            line_block.modals.spindleTurn = M5;
                            break;
                        case '6':
                            line_block.modals.toolChange = M6;
                            break;
                        case '7':                        
                            line_block.modals.coolant = M7;
                            break;
                        case '8':
                            line_block.modals.coolant = M8;
                            break;
                        case '9':
                            line_block.modals.coolant = M9;
                            break;
                        default:
                            break;
                    }
                }
				break;
			case 'R': case 'r':
            case 'P': case 'p':
            case 'F': case 'f':
            case 'S': case 's':
            case 'T': case 't':
            case 'X': case 'x':
            case 'Y': case 'y':
            case 'Z': case 'z':
            case 'I': case 'i':
            case 'J': case 'j':
            case 'K': case 'k':

                valueIndex = 0;
                value = 0;
                
                // Clear the value string buffer
                memset(valueString, 0, sizeof(valueString));

                do{
                    char_counter++;
                } while(line[char_counter] == ' ');
                

                // Store characters in valueString until whitespace or non-number character or end of line
                while (line[char_counter] != ' ' && line[char_counter] != '\0' && !isalpha(line[char_counter])) {
                    valueString[valueIndex++] = line[char_counter++];
                }

                valueString[valueIndex] = '\0'; // Null-terminate the string

                // Parse the value string to a float
                value = atof(valueString);

                // Update line_block based on the command
                if (isalpha(line[char_counter-valueIndex-1])){
                    updateBlockValue(line[char_counter-valueIndex-1],value);
                }
                else {
                    updateBlockValue(line[char_counter-valueIndex-2],value);
                }
                
                break;                    
               
			default:
                char_counter++;
                break;
    	}
    }





    // 2. set feed rate mode (G93, G94 — inverse time or per minute).
    if(line_block.modals.feed == G94){
        //set the feed rate in mm/min - convert to steps/second
        float feed_rate_interval = 0;
        feed_rate_interval = (1/((line_block.values.feed / (mm_per_revoltuion * 60))*steps_per_revoltuion))*1000000;
        if (feed_rate_interval <= min_delay_us){
            feed_rate_interval = (float)min_delay_us;
            line_block.values.feed = MAX_RPM;
        }
        //3. set feed rate (F).
        for(int i=0;i<3;i++){
            steppers[i].minStepInterval = feed_rate_interval;
        }

    }
    else if(line_block.modals.feed == G93){
        //set estimated time(seconds) for move with largest amount of 
        //steps then scale the rest as usual 

    }

    line_state.feed_rate = line_block.values.feed;
    printf("Feed rate: %.2f\n",(float)line_block.values.feed);


    //4. set spindle speed (S).
    setSpindleSpeed(line_block.values.spindle);
    line_state.spindle_speed = line_block.values.spindle;
        
    //5. select tool (T).
    line_state.tool = line_block.values.toolNo;
    //6. change tool (M6).
    if(line_block.modals.toolChange == M6){
        enableSpindle(0,0);
        printf("Tool change required! Paused until Enter is pressed\n\r");
        while(scanf("")==-1){};
        printf("Tool changed. Program will now resume.\n\r");
    }

    //7. spindle on or off (M3, M4, M5).
    switch(line_block.modals.spindleTurn){
        //clockwise at current programmed spindle speed
        case M3:
            enableSpindle(1,1);
            break;
        //CCW at current programmed spindle speed 
        case M4:
            enableSpindle(1,0);
            break;
        //stop spindle spinning
        case M5:
            enableSpindle(0,0);
            break;
        default:
            break;
    }


    //
  
    //8. coolant on or off (M7, M8, M9).
    if(line_state.modal.coolant != line_block.modals.coolant) {
        coolantEnable(line_block.modals.coolant);
        line_state.modal.coolant = line_block.modals.coolant;
    }

    //9. enable or disable overrides (M48, M49).
    // switch(line_block.modals.switchOverride){
    //     //enable feed and speed override switches
    //     case M48:
    //         break;
    //     //disable feed and speed override switches
    //     case M49:
    //         break;
    //     default:
    //         break;
    // }

    //10. dwell (G4).
    if(line_block.values.dwell != 0){
        wait_us(line_block.values.dwell*1000);
    }

    //11. set active plane (G17, G18, G19).
    switch(line_block.modals.plane){
        case G17:
            axis_0 = X_AXIS;
            axis_1 = Y_AXIS;
            axis_linear = Z_AXIS;
            break;
        case G18:
            axis_0 = Z_AXIS;
            axis_1 = X_AXIS;
            axis_linear = Y_AXIS;
            break;
        case G19:
            axis_0 = Y_AXIS;
            axis_1 = Z_AXIS;
            axis_linear = X_AXIS;
            break;
        default:
            break;
    }
    line_state.modal.plane = line_block.modals.plane;


    //12. set length units (G20, G21).
    if (line_block.modals.units == G20) {
        for (uint8_t idx=0; idx<N_AXIS; idx++) { // Axes indices are consistent, so loop may be used.
            if (axis_word & (1U << idx)) {
                line_block.values.xyz[idx] *= MM_PER_INCH;
            }
        }
    }
    line_state.modal.units = line_block.modals.units;


    //13. cutter radius compensation on or off (G40, G41, G42)
    // switch(line_block.modals.cutterRadius){
    //     case G40:
    //         break;
    //     case G41:
    //         break;
    //     case G42:
    //         break;
    //     default:
    //         break;
    // }


    //14. cutter length compensation on or off (G43, G49)
    
    line_state.modal.toolLength = line_block.modals.toolLength;
    switch(line_state.modal.toolLength){
        case G43:
            line_state.tool_length_offset = line_block.values.xyz[Z_AXIS];
            break;
        case G49:
            line_state.tool_length_offset = 0.0;
            break;
        default:
            break;
    }


    //15. coordinate system selection (G54, G55, G56, G57, G58, G59, G59.1, G59.2, G59.3).
    if (line_state.modal.coordSystem != line_block.modals.coordSystem) {
        line_state.modal.coordSystem = line_block.modals.coordSystem;
        memcpy(line_state.coord_system,coordinate_data,sizeof(coordinate_data));
    }

    // //16. set path control mode (G61, G61.1, G64)
    // switch(line_block.modals.pathControl){
    //     case G61:
    //         break;
    //     case G61_1:
    //         break;
    //     case G64:
    //         break;
    //     default:
    //         break;
    // }

    //17. set distance mode (G90, G91).
    line_state.modal.distance = line_block.modals.distance;


    // //18. set retract mode (G98, G99).
    // switch(line_block.modals.returnCanned){
    //     case disable:
    //         break;
    //     case G98:
    //         break;
    //     case G99:
    //         break;
    //     default:
    //         break;
    // }


    //19. home (G28, G30) or change coordinate system data (G10) or set axis offsets (G92, G92.1, G92.2, G92.3).
    //perform any non_modal commands within the line


    if( (line_block.nonModalFlag & (1U << G28)) | ( line_block.nonModalFlag & (1U << G30))){
        for (int i=0;i<N_AXIS;i++){
            steppers[i].minStepInterval = min_delay_us;
        }
        prepareMovement(1,line_block.values.xyz[X_AXIS]);
        prepareMovement(2,line_block.values.xyz[Y_AXIS]);
        prepareMovement(3,line_block.values.xyz[Z_AXIS]);
        runAndWait();

        prepareMovement(1,parameter_data[X_AXIS]);
        prepareMovement(2,parameter_data[Y_AXIS]);
        prepareMovement(3,parameter_data[Z_AXIS]);
        runAndWait();

        memcpy(line_state.position, parameter_data, sizeof(parameter_data));
        
    }

    else if( line_block.nonModalFlag & (1U << G10)){
        if (coord_select > 0) {
             coord_select--; 
        } // Adjust P1-P6 index to EEPROM coordinate data indexing.
        else { 
            coord_select = line_block.modals.coordSystem; 
        } // Index P0 as the active coordinate system

        // Pre-calculate the coordinate data changes. NOTE: Uses parameter_data since coordinate_data may be in use by G54-59.
        for (int idx=0; idx<N_AXIS; idx++) { // Axes indices are consistent, so loop may be used.
            // Update axes defined only in block. Always in machine coordinates. Can change non-active system.
            if (axis_word & (1U << idx)) {
                if (line_block.values.l == 20) {
                    // L20: Update coordinate system axis at current position (with modifiers) with programmed value
                    parameter_data[idx] = line_state.position[idx]-line_state.coord_offset[idx]-line_block.values.xyz[idx];
                    if (idx == Z_AXIS) { 
                        parameter_data[idx] -= line_state.tool_length_offset; 
                    }
                } 
                else {
                // L2: Update coordinate system axis to programmed value.
                parameter_data[idx] = line_block.values.xyz[idx]; 
                }
            }
        }


        //settings_write_coord_data(coord_select,parameter_data);
        // Update system coordinate system if currently active.
        if (line_state.modal.coordSystem == coord_select) { 
            memcpy(line_state.coord_system,parameter_data,sizeof(parameter_data)); 
        }
    }

    else if( line_block.nonModalFlag & (1U << G53)){

    }
    else if( line_block.nonModalFlag & (1U << G92)){
        // Update axes defined only in block. Offsets current system to defined value. Does not update when
        // active coordinate system is selected, but is still active unless G92.1 disables it. 
        for (int idx=0; idx<N_AXIS; idx++) { // Axes indices are consistent, so loop may be used.
            if (axis_word & ( 1U<<idx )) {
                line_block.values.xyz[idx] = line_state.position[idx]-coordinate_data[idx]-line_block.values.xyz[idx];
                if (idx == Z_AXIS) { 
                    line_block.values.xyz[idx] -= line_state.tool_length_offset; 
                }
            } 
            else {
                line_block.values.xyz[idx] = line_state.coord_offset[idx];
            }
        }


        memcpy(line_state.coord_offset,line_block.values.xyz,sizeof(line_block.values.xyz));
    }

    if (axis_word) {
        for (int idx=0; idx<N_AXIS; idx++) { // Axes indices are consistent, so loop may be used to save flash space.
            if ( !(axis_word &(1U<<idx))) {
                line_block.values.xyz[idx] = line_state.position[idx]; // No axis word in block. Keep same axis position.
            } 
            else {
                // Update specified value according to distance mode or ignore if absolute override is active.
                // NOTE: G53 is never active with G28/30 since they are in the same modal group.
                if (!(line_block.nonModalFlag & (1U << G53))) {
                    // Apply coordinate offsets based on distance mode.
                    if (line_block.modals.distance == G90) {
                        line_block.values.xyz[idx] += coordinate_data[idx] + line_state.coord_offset[idx];
                        if (idx == Z_AXIS) { 
                            line_block.values.xyz[idx] += line_state.tool_length_offset;
                        }
                    } 
                    else {  // Incremental mode
                        line_block.values.xyz[idx] += line_state.position[idx];
                    }
                }
            }
        }
    }

    else if( line_block.nonModalFlag & (1U << G92_1)){
        memset(line_state.coord_offset,0,sizeof(line_state.coord_offset));
    }

    else if( line_block.nonModalFlag & (1U << G28_1)){
        //settings_write_coord_data(SETTING_INDEX_G28,line_state.position);
        
    }

    else if( line_block.nonModalFlag & (1U << G30_1)){
        //settings_write_coord_data(SETTING_INDEX_G30,line_state.position);
    }   
    
    printf("Motion: %d\n",line_block.modals.motion);
    printf("X-axis target: %.2f\n",(float)line_block.values.xyz[X_AXIS]);
    printf("Y-axis target: %.2f\n",(float)line_block.values.xyz[Y_AXIS]);
    printf("Z-axis target: %.2f\n",(float)line_block.values.xyz[Z_AXIS]);
    printf("Feed rate: %.2f\n",(float)line_block.values.feed);
    printf("Spindle Speed: %.2f\n",(float)line_block.values.spindle);
    printf("Radius: %.2f\n",(float)line_block.values.radius);
    printf("Dwell: %.2f\n",(float)line_block.values.dwell);
    printf("Tool No: %d\n",line_block.values.toolNo);


    if(line_block.modals.motion == G0){
        if(!axis_word){
            printf("No coordinates for rapid move");
            return;
        }
    }
    else{
        switch(line_block.modals.motion){
            case G1:
                if(!axis_word){
                    printf("No coordinates for rapid move");
                    return;
                }
                break;
            case G2:case G3:
                if(!axis_word){
                    printf("No coordinates for rapid move");
                    return;
                }
                if (!(axis_word & ((1U<<axis_0)|(1U<<axis_1)))) { 
                    printf("No axis word in plane"); 
                    return;
                } // [No axis words in plane]
                // Calculate the change in position along each selected axis
                
                x = line_block.values.xyz[axis_0]-line_state.position[axis_0]; // Delta x between current position and target
                y = line_block.values.xyz[axis_1]-line_state.position[axis_1]; // Delta y between current position and target

                if(line_block.values.radius != 0){
                    if (line_state.position == line_block.values.xyz) {
                        printf("Invalid Target\n\r"); 
                        return;
                    } // [Invalid target]
                    if (line_block.modals.units == G20) {
                        line_block.values.radius *= MM_PER_INCH; 
                    }
                    float h_x2_div_d = (4.0 * (line_block.values.radius*line_block.values.radius)) - (x*x) - (y*y);
                    printf("%f",h_x2_div_d);
                    if (h_x2_div_d < 0) {
                        printf("Radius Error");
                        return; 
                    } // [Arc radius error]
        
                    // Finish computing h_x2_div_d.
                    h_x2_div_d = -sqrt(h_x2_div_d)/sqrt((x*x)+(y*y)); // == -(h * 2 / d)
                    // Invert the sign of h_x2_div_d if the circle is counter clockwise (see sketch below)
                    if (line_block.modals.motion == G3) {
                        h_x2_div_d = -h_x2_div_d; 
                    }  
                    if (line_block.values.radius < 0) { 
                            h_x2_div_d = -h_x2_div_d; 
                            line_block.values.radius = -line_block.values.radius; // Finished with r. Set to positive for mc_arc
                        }        
                    // Complete the operation by calculating the actual center of the arc
                    line_block.values.ijk[axis_0] = 0.5*(x-(y*h_x2_div_d));
                    line_block.values.ijk[axis_1] = 0.5*(y+(x*h_x2_div_d));
                    
                } 
                else { // Arc Center Format Offset Mode  
                    if (!(ijk_words & ((1U<<axis_0)|(1U<<axis_1)))) { 
                        printf("Gcode Error - No Offsets in Plane");
                        return;
                         
                    } // [No offsets in plane]
                    
                    
                
                    // Convert IJK values to proper units.
                    if (line_block.modals.units == G20) {
                        for (int idx=0; idx<N_AXIS; idx++) { // Axes indices are consistent, so loop may be used to save flash space.
                            if (ijk_words & (1U<<idx)) { 
                                line_block.values.ijk[idx] *= MM_PER_INCH; 
                            }
                        }
                    }         

                    // Arc radius from center to target
                    x -= line_block.values.ijk[axis_0]; // Delta x between circle center and target
                    y -= line_block.values.ijk[axis_1]; // Delta y between circle center and target
                    float target_r = sqrt((x*x)+(y*y)); 

                    // Compute arc radius for mc_arc. Defined from current location to center.
                    line_block.values.radius = sqrt((line_block.values.ijk[axis_0] * line_block.values.ijk[axis_0]) + (line_block.values.ijk[axis_1] * line_block.values.ijk[axis_1]) ); 
                    
                    // Compute difference between current location and target radii for final error-checks.
                    float delta_r = fabs(target_r-line_block.values.radius);
                    if (delta_r > 0.005) { 
                        if (delta_r > 0.5) {
                            printf("Gcode error - Invalid Target\n\r"); 
                            return;
                        } // [Arc definition error] > 0.5mm
                        if (delta_r > (0.001*line_block.values.radius)) {
                            printf("Gcode error - Invalid Target\n\r"); 
                            return;
                        } // [Arc definition error] > 0.005mm AND 0.1% radius
                    }
                }
                break;
            case G38_2:
                if(!axis_word){
                    printf("No coordinates for rapid move");
                    return;
                }
                if (line_state.position == line_block.values.xyz) {
                    printf("Invalid Target\n\r"); 
                    return;
                } // [Invalid target]
                break;
            default:
                break;
        }
    }


    //20. perform motion (G0 to G3, G80 to G89), as modified (possibly) by G53.
    switch(line_block.modals.motion){
        //perform traverse move to location
        case G0:
            for (int i = 0;i<3;i++){
                steppers[i].minStepInterval = min_delay_us;
            }
            prepareMovement(1, (float)line_block.values.xyz[X_AXIS]);
            prepareMovement(2, (float)line_block.values.xyz[Y_AXIS]);
            prepareMovement(3, (float)line_block.values.xyz[Z_AXIS]);
            runAndWait();
            break;
        case G1:
            prepareMovement(1,(float) line_block.values.xyz[X_AXIS]);
            prepareMovement(2, (float)line_block.values.xyz[Y_AXIS]);
            prepareMovement(3, (float)line_block.values.xyz[Z_AXIS]);
            runAndWait();
            break;
        case G2:
            //motionArc(line_state.position, line_block.values.xyz, line_block.values.ijk, line_block.values.radius, 
                //line_state.feed_rate, line_state.modal.feed, axis_0, axis_1, axis_linear, true);
            break;
        case G3:
            //motionArc(line_state.position, line_block.values.xyz, line_block.values.ijk, line_block.values.radius, 
                //line_state.feed_rate, line_state.modal.feed, axis_0, axis_1, axis_linear, false);
            break;
        case G38_2:
            //probeCycle(line_block.values.xyz, line_state.feed_rate, line_state.modal.feed);
            break;
        default:
            break;
    }
    line_state.position[X_AXIS] = line_block.values.xyz[X_AXIS];
    line_state.position[Y_AXIS] = line_block.values.xyz[Y_AXIS];
    line_state.position[Z_AXIS] = line_block.values.xyz[Z_AXIS];
    printf("X-axis Position: %.2f\n",(float)line_state.position[X_AXIS]);
    printf("Y-axis Position: %.2f\n",(float)line_state.position[Y_AXIS]);
    printf("Z-axis Position: %.2f\n",(float)line_state.position[Z_AXIS]);
    //memcpy(line_state.position, line_block.values.xyz, sizeof(line_block.values.xyz)); // gc_state.position[] = gc_block.values.xyz[]




    // [21. Program flow ]:
    // M0,M1,M2,M30: Perform non-running program flow actions. During a program pause, the buffer may 
    // refill and can only be resumed by the cycle start run-time command.
    line_state.modal.stopping = line_block.modals.stopping;
    if (line_state.modal.stopping) { 
	    //protocol_buffer_synchronize(); // Sync and finish all remaining buffered motions before moving on.
	    // if (line_state.modal.stopping == (M0||M1)) {
	    //     if (sys.state != STATE_CHECK_MODE) {
		//         //bit_true_atomic(sys_rt_exec_state, EXEC_FEED_HOLD); // Use feed hold for program pause.
		//         //protocol_execute_realtime(); // Execute suspend.
	    //     }
        // } else { // == PROGRAM_FLOW_COMPLETED
            // Upon program complete, only a subset of g-codes reset to certain defaults, according to 
            // LinuxCNC's program end descriptions and testing. Only modal groups [G-code 1,2,3,5,7,12]
            // and [M-code 7,8,9] reset to [G1,G17,G90,G94,G40,G54,M5,M9,M48]. The remaining modal groups
            // [G-code 4,6,8,10,13,14,15] and [M-code 4,5,6] and the modal words [F,S,T,H] do not reset.
            line_state.modal.motion = G1;
            line_state.modal.plane = G17;
            line_state.modal.distance = G90;
            line_state.modal.feed = G94;
            // line_state.modal.cutter_comp = CUTTER_COMP_DISABLE; // Not supported.
            line_state.modal.coordSystem = G54; // G54
            line_state.modal.spindleTurn = M5;
            line_state.modal.coolant = M9;
            // line_state.modal.override = OVERRIDE_DISABLE; // Not supported.
            
            // Execute coordinate change and spindle/coolant stop.

            // if (!(settings_read_coord_data(line_state.modal.coordSystem,coordinate_data))) { 
            //     printf("Failed to read coordinate data settings from eeprom");
            //     return;                    
            // } 
            memcpy(line_state.coord_system,coordinate_data,sizeof(coordinate_data));
            //enableSpindle(0);
            //coolant_stop();		
            
            
            printf("Program Ended");
        
        line_state.modal.stopping = resume; // Reset program flow.
    }

}