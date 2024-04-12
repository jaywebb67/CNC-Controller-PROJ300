#include "peripherals.hpp"


InterruptIn stopButton(safetydoorPin);
InterruptIn holdButton(holdButtonPin);
InterruptIn safetyDoor(safetydoorPin);

uint8_t holdFlag = 0;

void stopISR();
void holdISR();
void safetyDoorISR();

void peripheralsInit(){
    holdFlag = 0;
    stopButton.fall(stopISR);
    holdButton.fall(holdISR);
    safetyDoor.fall(safetyDoorISR);
}

void stopISR(){
    wait_us(20000);
    system_reset(); 
}

void holdISR(){
    wait_us(20000);
    holdFlag ^= STATE_hold;
}

void safetyDoorISR(){
    wait_us(20000);
    holdFlag ^= STATE_safetyDoor;
}