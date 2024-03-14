
#include "steppers.h"


void x_stepISR(){

    
}

void y_stepISR(){


}

void z_stepISR(){


}

void stepperInit(int frequency){

    
    DigitalInOut stepperEN(stepperEN_pin,PinDirection::PIN_OUTPUT,PinMode::OpenDrainNoPull,1);
    DigitalOut x_dir(xdir_pin);
    DigitalOut y_dir(ydir_pin);
    DigitalOut z_dir(zdir_pin);

    PwmOut x_step(xstep_pin);
    PwmOut y_step(ystep_pin);
    PwmOut z_step(zstep_pin);

    float period = (1.0/frequency)*1000000;

    x_step.period_us(period);
    x_step.write(0.5f);
    x_step.rise(&x_stepISR);



}

