#include "spindle.hpp"


PwmOut spindleA(spindleA_pin);

PwmOut spindleB(spindleB_pin);

DigitalOut spindleEN(spindleEN_pin);

float spindleSpeed = 0.0f;


void spindleInit(){
    spindleEN = 0;

    spindleA.period_us(10);
    spindleA = 0.0f;

    spindleB.period_us(10);
    spindleB = 0.0f;


}


void enableSpindle(bool EN, bool dir){
    float duty = 0.0f;
    if(EN){
        spindleA = 0.0f; spindleB = 0.0f;
        spindleEN = 1;
        if (dir){
            while(duty<=spindleSpeed){
                duty = duty + spindleAccel;
                spindleA = duty;
                wait_us(10);
            }
        }
        else{
            while(duty<=spindleSpeed){
                duty = duty + spindleAccel;
                spindleB = duty;
                wait_us(10);
            }
        }
    }
    else{
        spindleA = 0.0f; spindleB = 0.0f;
        spindleEN = 0;
    }
}

void setSpindleSpeed(short speed){
    if(speed>=1000){
        spindleSpeed = 1;
    }
    else if(speed<=0){
        spindleSpeed = 0;
    }
    else{
        spindleSpeed = (speed*1.0f)/1000;
    }

}
