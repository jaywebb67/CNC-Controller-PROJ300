#include "mbed.h"

#include "steppers.hpp"
#include "jogButtons.hpp"
#include "clock_config.hpp"




// main() runs in its own thread in the OS
int main()
{
    clock_config180();
    SystemCoreClockUpdate();

    
    bool skip = 0;
    int dir = 0;
    int axis = 0;
    uint32_t steps = 0;
    uint32_t frequency = 0;
    uint16_t microstepping = 8;
    int mode;


    printf("\n\rEnter the desired PWM frequency: " );
    
    if(scanf("%d",&frequency) ==1){
        printf("%d \n\r",frequency);
    }

    else{
        printf("Failed to scan frequency input\n\r");
    }


    stepperInit(static_cast<float>(frequency));
    jogInit();

    printf("\n\rSelect mode of operation (1 = Jog, 2 = MDI): ");
    scanf("%d",&mode);
    printf("%d\n\r",mode);


    while (true) {

        if ((mode == 1)&(skip == 0)){
            jogEnable(1);
            skip = 1;

        }
        else if ((mode == 2)&(skip == 0)) {

            printf("Select which axis to move (X,Y,Z): ");
            scanf("%d",&axis);
            printf("%d\n\r",axis);

            printf("Select the amount of steps to take: ");
            scanf("%d",&steps);
            printf("%d\n\r",steps);

            coordinatedMotion(steps,max_velocity);
        }
        else{
            ThisThread::sleep_for(5s);
        }

        

    }
}

