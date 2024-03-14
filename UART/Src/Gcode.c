/*

 * Gcode.c
 *
 *  Created on: Dec 20, 2023
 *      Author: jaywe


#include "Gcode.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void parse_gcode(char *line, line_t line_modals) {

	uint8_t char_counter = 0;

    while (char_counter <= MAX_CHARACTER_PER_LINE) {
    	switch(line[char_counter]){
			case 'G':
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

    	}
        if (line[char_counter] == 'G') {
            // Handle G-code command
            int code = atoi(&token[1]);
            switch (code) {
                case 0:
                    printf("\n\rG0 rapid move\n\r");
                    break;
                case 1:
                    printf("\n\rG1 linear move\n\r");
                    break;
                case 2:
                    printf("\n\rG2 Circular move\n\r");
                    break;
                // Add more cases for other G-code commands
                default:
                    printf("\n\rUnknown G-code command: %s\n\r", token);
            }
        } else if (token[0] == 'M') {
            // Handle M-code command
            int code = atoi(&token[1]);
            switch (code) {
                case 3:
                    printf("\n\rM3 spindle on, clockwise rotation\n");
                    break;
                case 5:
                    printf("\n\rM5 spindle off\n\r");
                    break;
                // Add more cases for other M-code commands
                default:
                    printf("\n\rUnknown M-code command: %s\n\r", token);
            }
        } else if (token[0] == ';') {
            // Ignore comment
            break;
        }else if (token[0] == 'X') {
            // Ignore comment
            printf("\n\rX-axis Location\n\r");
        }else if (token[0] == 'Y') {
            // Ignore comment
            printf("\n\rY-axis Location\n\r");
        }else if (token[0] == 'Z') {
            // Ignore comment
            printf("\n\rZ-axis Location\n\r");
        } else {
            // Handle other tokens (e.g. X, Y, Z, F)
            printf("\n\rToken: %s\n\r", token);
        }
        token = strtok(NULL, " ");
    }
}



*/
