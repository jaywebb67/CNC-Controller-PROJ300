#include "mbed.h"

#include "steppers.hpp"
#include "jogButtons.hpp"
#include "Gcode.hpp"
#include <cstdint>
#include <cstdio>
#include <string>
#include "settings.hpp"
#include "probe.hpp"
#include "coolant.hpp"
#include "spindle.hpp"
#include "SDcard.hpp"
#include "limits.hpp"
#include "peripherals.hpp"



int main()
{

    volatile int char_counter = 0;
    volatile int mode = 0;
    char *userInput = (char *)malloc(MAX_CHARACTER_PER_LINE * sizeof(char)); // Allocate memory for input
    if (userInput == NULL) {
        printf("Memory allocation failed.");
        return 1;
    }


    settings_init();
    spindleInit();
    stepperInit();
    limitsInit();
    jogInit();
    probeInit();
    peripheralsInit();
    sdCard_Init();
    coolantInit();
    homeCycle();

    printf( "------------------------------------\r\n"
            "Welcome to STM32 CNC Controller V1.0\r\n"
            "------------------------------------\r\n");
            

    while (true) {
        char_counter = 0;
        printf( "Main Menu:\r\n"
                "$$ ------- (view settings)\r\n"
                "$# ------- (view # parameters)\r\n"
                "$G ------- (view parser state)\r\n"
                "$x=value - (save setting)\r\n"
                "$H ------- (run homing cycle)\r\n"
                "$S ------- (SD card)\r\n"
                "$J ------- (Jogging)\r\n"
                "$MDI ----- (Manual Data Input)\r\n");

        scanf(" %[^\n]",userInput);
        while(userInput[char_counter] != '\0'){
            switch(userInput[char_counter]){
                case '$':
                    char_counter++;
                    if(userInput[char_counter] == ' '){
                        char_counter++;
                    }
                    switch(userInput[char_counter]){
                        case '$':
                            reportSettings();
                            break;
                        case '#':
                            report_ngc_parameters();
                            break;
                        case 'G':
                            reportParserModes();
                            break;
                        case 'H':
                            homeCycle();
                            break;
                        case 'S':
                            sdCardMode();
                            break;
                        case 'J':
                            jogEnable(1);
                            break;
                        case 'M':
                            gCodeMode();
                        default:
                            char parameterStr[5];
                            char* endPtr;
                            char valueStr[20];
                            uint8_t valueIndex = 0;
                            while(userInput[char_counter] == ' '){
                                char_counter++;
                            }
                            // Store characters in valueString until whitespace or non-number character or end of line
                            while (userInput[char_counter] != ' ' && userInput[char_counter] != '\0' && !isalpha(userInput[char_counter])) {
                                if(userInput[char_counter] == '='){
                                    char_counter++;
                                    break;
                                }
                                parameterStr[valueIndex++] = userInput[char_counter++];
                            }
                            while (userInput[char_counter] != ' ' && userInput[char_counter] != '\0' && !isalpha(userInput[char_counter])) {
                                valueStr[valueIndex++] = userInput[char_counter++];
                            }
                            uint16_t parameter = (uint16_t)strtoul(parameterStr, &endPtr, 10);

                            // Check for conversion errors
                            if (*endPtr != '\0') {
                                printf("Error: Invalid input\n");
                            } 
                            
                            float value = strtof(valueStr, &endPtr);

                            // Check if conversion was successful
                            if (*endPtr != '\0') {
                                // Conversion failed, handle error
                                printf("Error: Invalid numerical value '%s'\n", valueStr);
                            }
                            settings_store_global_setting(parameter, value);
                            break;
                    }
                    break;
                default:
                    printf("Incorrect input format, Re-enter according to how menu displayed!\r\n");
                    break;
            }
            char_counter++;
        }

    }
    free(userInput);
}