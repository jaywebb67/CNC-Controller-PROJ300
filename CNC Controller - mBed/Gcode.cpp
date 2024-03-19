 /*
 * Gcode.c
 *
 *  Created on: Dec 20, 2023
 *      Author: jaywe
*/

#include "Gcode.hpp"




void parse_gcode(char *line, line_t line_modals) {

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
                else{
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
                                        line_modals.nonModal[char_counter] = G28;
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
                                        line_modals.nonModal[char_counter] = G30;
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
                                    line_modals.nonModal[char_counter] = G53;
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
                                
                                        line_modals.coordSystem = G59;
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
                                        line_modals.nonModal[char_counter] = G92;
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
                                    break;
                                default:
                                    break;
                            }
                            break;
                        default:
                            break;
                    }
                }
                break;
			case 'M':

				break;
			case 'R':
				break;
			case 'D':
				break;
			case 'F':
				break;
			case 'S':
				break;
			case 'T':
				break;
			case 'N':
				break;
			case 'X':
				break;
			case 'Y':
				break;
			case 'Z':
				break;
			default:
                break;
    	}
        char_counter++;

    }
    executeLine(line_modals, 0, 0, 0);
}




