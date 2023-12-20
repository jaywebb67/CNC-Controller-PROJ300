/*
 * Gcode.c
 *
 *  Created on: Dec 20, 2023
 *      Author: jaywe
 */

#include "Gcode.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void parse_gcode(char *line) {
    char *token;
    //char delimiters[] = " "; // Delimiters include space, M, X, Y, Z
    token = strtok(line," ");
    while (token != NULL) {
        if (token[0] == 'G') {
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



