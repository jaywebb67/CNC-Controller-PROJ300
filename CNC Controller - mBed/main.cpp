#include "mbed.h"

#include "steppers.hpp"
#include "jogButtons.hpp"
#include "Gcode.hpp"
#include <cstdio>
#include <string>
//#include "clock_config.hpp"

// Thread motorMovement;
// EventQueue motorQueue;


// main() runs in its own thread in the OS
int main()
{
    //motorMovement.start(callback(&motorQueue, &EventQueue::dispatch_forever));
    volatile bool skip = 0;
    volatile int axis = 0;
    volatile long steps = 0;
    volatile int acceleration = 0;
    volatile int max_speed = 0;
    volatile int mode = 0;
    
    char *g_codeLine = (char *)malloc(MAX_CHARACTER_PER_LINE * sizeof(char)); // Allocate memory for input
    if (g_codeLine == NULL) {
        printf("Memory allocation failed.");
        return 1;
    }
    printf("\n\rEnter the acceleration rate: " );
    
    if(scanf("%d",&acceleration) ==1){
        printf("%d \n\r",acceleration);
    }

    else{
        printf("Failed to scan acceleration input\n\r");
    }

    printf("\n\rEnter the max speed: " );
    
    if(scanf("%d",&max_speed) ==1){
        printf("%d \n\r",max_speed);
    }

    else{
        printf("Failed to scan max_speed input\n\r");
    }

    stepperInit(acceleration,max_speed);
    jogInit();

    printf("\n\rSelect mode of operation (1 = Jog, 2 = MDI): ");
    scanf("%d",&mode);
    printf("%d\n\r",mode);


    while (true) {

        if ((mode == 1)&&(skip == 0)){
            jogEnable(1);
            skip = 1;

        }
        else if ((mode == 2)&&(skip == 0)) {

            printf("Enter G-code line: ");
            fscanf(stdin," %[^\n]",g_codeLine);
            printf("%s\n\r", g_codeLine);
            parse_gcode(g_codeLine);

            // printf("Select which axis to move (X,Y,Z): ");
            // scanf("%d",&axis);
            // printf("%d\n\r",axis);

            // printf("Select the amount of steps to take: ");
            // scanf("%ld",&steps);
            // printf("%ld\n\r",steps);

            // prepareMovement(axis, steps);
            // prepareMovement(2, 2* steps);
            // prepareMovement(3, 3* steps);
            // runAndWait();
        }
        else{
            ThisThread::sleep_for(5s);
        }
    }
    free(g_codeLine);
}