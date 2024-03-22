 /*
 * Gcode.c
 *
 *  Created on: Dec 20, 2023
 *      Author: jaywe
*/

#include "Gcode.hpp"




void parse_gcode(char *line, lineModals_t line_modals) {

    lineStruct_t lineStructure;
	int char_counter = 0;
    int nonModalCount = 0;
    while (char_counter <= MAX_CHARACTER_PER_LINE) {
    	switch(line[char_counter]){
            case ' ':
                break;
			case 'G':
                char_counter++;
                if(line[char_counter] == ' '){
                    break;
                }
                switch(line[char_counter]){
                    case 0:
                        line_modals.motion = G0;
                        break;
                    case 1:
                        char_counter++;
                        if(line[char_counter] == ' '){
                            line_modals.motion = G1;
                            break;
                        }
                        else{
                            switch(line[char_counter]){
                                case 0:
                                    line_modals.nonModal[nonModalCount] = G10;
                                    nonModalCount++;
                                    break;
                                case 7:
                                    line_modals.plane = G17;
                                    break;
                                case 8:
                                    line_modals.plane = G18;
                                    break;
                                case 9:
                                    line_modals.plane = G19;
                                    break;
                                default:
                                    break;
                            }
                        }

                    case 2:
                        char_counter++;
                        if(line[char_counter] == ' '){
                            line_modals.motion = G2;
                            break;
                        }
                        else{
                            switch(line[char_counter]){
                                case 0:
                                    line_modals.units = G20;
                                    break;
                                case 1:
                                    line_modals.units = G21;
                                    break;
                                case 8:
                                    line_modals.nonModal[nonModalCount] = G28;
                                    nonModalCount++;
                                    break;
                                default:
                                    break;
                            }
                        }
                    case 3:
                        char_counter++;
                        if(line[char_counter] == ' '){
                            line_modals.motion = G3;
                        }
                        else{
                            switch(line[char_counter]){
                                case 0:
                                    line_modals.nonModal[nonModalCount] = G30;
                                    nonModalCount++;
                                    break;
                                case 8:
                                    line_modals.motion = G38_2;
                                    break;
                                default:
                                    break;
                            }
                        }
                        break;
                    case 4:
                        char_counter++;
                        if(line[char_counter] == ' '){
                            line_modals.nonModal[nonModalCount] = G4;
                            nonModalCount++;
                            break;
                        }
                        else{
                            switch(line[char_counter]){
                                case 0:
                                    line_modals.cutterRadius = G40;
                                    break;
                                case 1:
                                    line_modals.cutterRadius = G41;
                                    break;
                                case 2:
                                    line_modals.cutterRadius = G42;
                                    break;
                                case 3:
                                    line_modals.toolLength = G43;
                                    break;
                                case 9:
                                    line_modals.toolLength = G49;
                                    break;
                                default:
                                    break;
                            }
                        }  
                        break;                          
                    case 5:
                        char_counter++;
                        switch(line[char_counter]){
                            case 3:
                                line_modals.nonModal[nonModalCount] = G53;
                                nonModalCount++;
                                break;
                            case 4:
                                line_modals.cutterRadius = G41;
                                break;
                            case 5:
                                line_modals.cutterRadius = G42;
                                break;
                            case 6:
                                line_modals.coordSystem = G56;
                                break;
                            case 7:
                                line_modals.coordSystem = G57;
                                break;
                            case 8:
                                line_modals.coordSystem = G58;
                                break;
                            case 9:
                                char_counter++;
                                if(line[char_counter] == ' '){
                                    line_modals.coordSystem = G59;
                                }
                                else{
                                    char_counter++;
                                    switch (line[char_counter]) {
                                        case 1:
                                            line_modals.coordSystem = G59_1;
                                            break;
                                        case 2:
                                            line_modals.coordSystem = G59_2;
                                            break;
                                        case 3:
                                            line_modals.coordSystem = G59_3;
                                            break;
                                        default:
                                            break;
                                    }
                            
                                    
                                    break;
                                }
                            default:
                                break;
                        }
                        break;
                    case 6:
                        char_counter++;
                        if (line[char_counter] == 1){
                            char_counter++;
                            if(line[char_counter] == ' '){
                                line_modals.pathControl = G61;                                
                            }
                            else{
                                line_modals.pathControl = G61_1;
                                char_counter++;
                            }
                        }
                        else if (line[char_counter] == 4) {
                            line_modals.pathControl = G64;
                        }
                        break;
                    case 8:
                        char_counter++;
                        switch (line[char_counter]) {
                            case 0:
                                line_modals.motion = G80;
                                break;
                            case 1:
                                line_modals.motion = G81;
                                break;
                            case 2:
                                line_modals.motion = G82;
                                break;
                            case 3:
                                line_modals.motion = G83;
                                break;
                            case 4:
                                line_modals.motion = G84;
                                break;
                            case 5:
                                line_modals.motion = G85;
                                break;
                            case 6:
                                line_modals.motion = G86;
                                break;
                            case 7:
                                line_modals.motion = G87;
                                break;
                            case 8:
                                line_modals.motion = G88;
                                break;
                            case 9:
                                line_modals.motion = G89;
                                break;
                            default:
                                break;
                        }
                        break;
                    case 9:
                        char_counter++;
                        switch(line[char_counter]){
                            case 0:
                                line_modals.distance = G90;
                                break;
                            case 1:
                                line_modals.distance = G91;
                                break;
                            case 3:
                                line_modals.feed = G93;
                                break;
                            case 4:
                                line_modals.feed = G93;
                                break;
                            case 8:
                                line_modals.returnCanned = G98;
                                break;
                            case 9:
                                line_modals.returnCanned = G99;
                                break;
                            case 2:
                                char_counter++;
                                if (line[char_counter] == ' '){
                                    line_modals.nonModal[nonModalCount] = G92;
                                    nonModalCount++;
                                    break;
                                }
                                else{
                                    char_counter++;
                                    switch(line[char_counter]){
                                        case 1:
                                            line_modals.nonModal[char_counter] = G92_1;
                                            break;
                                        case 2:
                                            line_modals.nonModal[char_counter] = G92_2;
                                            break;
                                        case 3:
                                            line_modals.nonModal[char_counter] = G92_3;
                                            break;
                                        default:
                                            break;
                                    }
                                }
                            default:
                                break;
                        }
                        break;
                    default:
                        break;
                }
                break;
			case 'M':
                char_counter++;
                if(line[char_counter] == ' '){
                    break;
                }
                else{
                    switch (line[char_counter]) {
                        case 0:
                            line_modals.stopping = M0;
                            break;
                        case 1:
                            line_modals.stopping = M1;
                            break;
                        case 2:
                            line_modals.stopping = M2;
                            break;
                        case 3:
                            char_counter++;
                            if (line[char_counter] == ' '){
                                line_modals.spindleTurn = M3;
                                break;
                            }
                            else if (line[char_counter] == 0) {
                                    line_modals.stopping = M30;
                                }                            
                            break;
                        case 4:
                            char_counter++;
                            if (line[char_counter] == ' '){
                                line_modals.spindleTurn = M4;
                            }
                            else if (line[char_counter] == 8) {
                                line_modals.switchOverride = M48;
                            }
                            else if(line[char_counter]){
                                line_modals.switchOverride = M49;
                            }
                            break;
                        case 5:
                            line_modals.spindleTurn = M5;
                            break;
                        case 6:
                            line_modals.toolChange = M6;
                            break;
                        case 7:
                            if (line_modals.coolant == M8){
                                line_modals.coolant = M7M8;
                            }
                            else{
                                line_modals.coolant = M7;
                            }
                            break;
                        case 8:
                            if (line_modals.coolant == M7){
                                line_modals.coolant = M7M8;
                            }
                            else{
                                line_modals.coolant = M8;
                            }                            
                            break;
                        case 9:
                            line_modals.coolant = M9;
                            break;
                        default:
                            break;
                    }
                }
				break;
			case 'R':
                char_counter++;
                while(line[char_counter] != ' '){
                    if (isdigit(line[char_counter])) {
                        // Convert character to integer and add to the number
                        if(line[char_counter-1] == '.'){
                            lineStructure.radius = lineStructure.radius + ((line[char_counter] - '0') * 0.1);
                        }
                        else{
                            lineStructure.radius = lineStructure.radius * 10 + (line[char_counter] - '0');
                        }
                    }
                    char_counter++;
                }
				break;
			case 'D':
                char_counter++;
                while(line[char_counter] != ' '){
                    if (isdigit(line[char_counter])) {
                        // Convert character to integer and add to the number
                        if(line[char_counter-1] == '.'){
                            lineStructure.dwell = lineStructure.dwell + ((line[char_counter] - '0') * 0.1);
                        }
                        else{
                            lineStructure.dwell = lineStructure.dwell * 10 + (line[char_counter] - '0');
                        }
                    }
                    char_counter++;
                }
				break;
			case 'F':
                char_counter++;
                while(line[char_counter] != ' '){
                    if (isdigit(line[char_counter])) {
                        // Convert character to integer and add to the number
                        if(line[char_counter-1] == '.'){
                            lineStructure.feed = lineStructure.feed + ((line[char_counter] - '0') * 0.1);
                        }
                        else{
                            lineStructure.feed = lineStructure.feed * 10 + (line[char_counter] - '0');
                        }
                    }
                    char_counter++;
                }
				break;
			case 'S':
                char_counter++;
                while(line[char_counter] != ' '){
                    if (isdigit(line[char_counter])) {
                        // Convert character to integer and add to the number
                        if(line[char_counter-1] == '.'){
                            lineStructure.spindle = lineStructure.spindle + ((line[char_counter] - '0') * 0.1);
                        }
                        else{
                            lineStructure.spindle = lineStructure.spindle * 10 + (line[char_counter] - '0');
                        }
                    }
                    char_counter++;
                }
				break;
			case 'T':
                char_counter++;
                while(line[char_counter] != ' '){
                    if (isdigit(line[char_counter])) {
                        // Convert character to integer and add to the number
                        lineStructure.radius = lineStructure.radius * 10 + (line[char_counter] - '0');
                    }
                    char_counter++;
                }
				break;
			case 'X':
                char_counter++;
                while(line[char_counter] != ' '){
                    if (isdigit(line[char_counter])) {
                        // Convert character to integer and add to the number
                        if(line[char_counter-1] == '.'){
                            lineStructure.x_target = lineStructure.x_target + ((line[char_counter] - '0') * 0.1);
                        }
                        else{
                            lineStructure.x_target = lineStructure.x_target * 10 + (line[char_counter] - '0');
                        }
                    }
                    char_counter++;
                }
				break;
			case 'Y':
                char_counter++;
                while(line[char_counter] != ' '){
                    if (isdigit(line[char_counter])) {
                        // Convert character to integer and add to the number
                        if(line[char_counter-1] == '.'){
                            lineStructure.y_target = lineStructure.y_target + ((line[char_counter] - '0') * 0.1);
                        }
                        else{
                            lineStructure.y_target = lineStructure.y_target * 10 + (line[char_counter] - '0');
                        }
                    }
                    char_counter++;
                }
				break;
			case 'Z':
                char_counter++;
                while(line[char_counter] != ' '){
                    if (isdigit(line[char_counter])) {
                        // Convert character to integer and add to the number
                        if(line[char_counter-1] == '.'){
                            lineStructure.z_target = lineStructure.z_target + ((line[char_counter] - '0') * 0.1);
                        }
                        else{
                            lineStructure.z_target = lineStructure.z_target * 10 + (line[char_counter] - '0');
                        }
                    }
                    char_counter++;
                }
				break;
			default:
                break;
    	}
        char_counter++;

    }
    lineStructure.line_modals = line_modals;
    executeLine(lineStructure);
}


void executeLine(lineStruct_t line_structure){
    printf("X-axis target: %f",line_structure.x_target);
    printf("Y-axis target: %f",line_structure.y_target);
    printf("Z-axis target: %f",line_structure.z_target);
    printf("Feed ratet: %f",line_structure.feed);
    printf("Spindle Speed: %f",line_structure.spindle);


    // 2. set feed rate mode (G93, G94 — inverse time or per minute).
    if(line_structure.line_modals.feed == G94){
        //set the feed rate in mm/min - convert to steps/second
        int feedRate = (1/((line_structure.feed / (mm_per_revoltuion * 60))*steps_per_revoltuion))*1000000;
        //3. set feed rate (F).
        for(int i=0;i<3;i++){
            steppers[i].minStepInterval = feedRate;
        }
    }
    else if(line_structure.line_modals.feed == G93){
        //set estimated time(seconds) for move with largest amount of 
        //steps then scale the rest as usual 

    }

    




    
/*  4. set spindle speed (S).
    5. select tool (T).
    6. change tool (M6).
    7. spindle on or off (M3, M4, M5).
    8. coolant on or off (M7, M8, M9).
    9. enable or disable overrides (M48, M49).
    10. dwell (G4).
    11. set active plane (G17, G18, G19).
    12. set length units (G20, G21).
    13. cutter radius compensation on or off (G40, G41, G42)
    14. cutter length compensation on or off (G43, G49)
    15. coordinate system selection (G54, G55, G56, G57, G58, G59, G59.1, G59.2, G59.3).
    16. set path control mode (G61, G61.1, G64)
    17. set distance mode (G90, G91).
    18. set retract mode (G98, G99).
    19. home (G28, G30) or change coordinate system data (G10) or set axis offsets (G92, G92.1, G92.2, G94).
    20. perform motion (G0 to G3, G80 to G89), as modified (possibly) by G53.
    21. stop (M0, M1, M2, M30, M60). */


}