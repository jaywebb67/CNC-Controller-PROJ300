#include "coolant.hpp"

#include "Gcode.hpp"

DigitalOut coolantFlood(coolantFlood_PIN);
DigitalOut coolantMist(coolantMist_PIN);

void coolantInit(){

    coolantFlood = 0;
    coolantMist = 0;

}


void coolantEnable(uint8_t type){

    switch(type){
        case M7:
            GPIOF->BSRR = (1U<<(mistPin_No));
            break;
        case M8:
            //enable coolant flood pin
            GPIOF->BSRR = (1U<<(floodPin_No));
            break;
        case M9:
            //reset both pins atomically using the Bit Set/Reset Register
            GPIOF->BSRR = (1U<<(mistPin_No+16)|(1U<<(floodPin_No+16)));
            break;
        default:
            break;
    }
}




