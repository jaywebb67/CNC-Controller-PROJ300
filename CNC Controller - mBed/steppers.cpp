#include "steppers.hpp"
#include <cstdint>

DigitalInOut stepperEN(stepperEN_pin,PinDirection::PIN_OUTPUT,PinMode::OpenDrainNoPull,1);
DigitalOut x_dir(xdir_pin);
DigitalOut y_dir(ydir_pin);
DigitalOut z_dir(zdir_pin);


Ticker xTimer;
DigitalOut x_step(xstep_pin,0);

Ticker yTimer;
DigitalOut y_step(ystep_pin,0);

Ticker zTimer;
DigitalOut z_step(zstep_pin,0);
// InterruptIn y_stepper(ystep_pin);
// PwmOut y_step(ystep_pin);

// InterruptIn z_stepper(zstep_pin);
// PwmOut z_step(zstep_pin);

Thread motorMovementX;
EventQueue motorQueueX;

Thread motorMovementY;
EventQueue motorQueueY;

Thread motorMovementZ;
EventQueue motorQueueZ;

motionPhase_t motion_phase;

bool countSteps = 0;


//volatile float period_us =625; // Initial period

void x_stepISR();
void y_stepISR();
void z_stepISR();



void stepperInit(float frequency){

    x_stepper.rise(NULL);
    x_step.period(1.0/(frequency));
    x_step.write(0.5);
    x_step.suspend();

    // y_stepper.rise(NULL);
    // y_step.period(1.0/(frequency));
    // y_step.write(0.5);
    // y_step.suspend();

    // z_stepper.rise(NULL);
    // z_step.period(1.0/(frequency));
    // z_step.write(0.5);
    // z_step.suspend();


    stepperEN = 1;
    countSteps = 0;

    motorMovementX.start(callback(&motorQueueX, &EventQueue::dispatch_forever));
    motorMovementY.start(callback(&motorQueueY, &EventQueue::dispatch_forever));
    motorMovementZ.start(callback(&motorQueueZ, &EventQueue::dispatch_forever));
}



void move_axis_stepper_motor(int axis, uint32_t steps, int dir){

    stepper_direction(axis,dir);
//	axis = toupper(axis);
	switch(axis){
        case 1:
			enableSteppers(2,0);
            enableSteppers(3,0);
            enableSteppers(1,1);
			xDesiredSteps = steps;
			x_steps_taken = 0;
			break;
        case 2:
			enableSteppers(1,0);
            enableSteppers(3,0);
            enableSteppers(2,1);
			yDesiredSteps = steps;
			y_steps_taken = 0;
			break;
		case 3:
			enableSteppers(1,0);
            enableSteppers(2,0);
            enableSteppers(3,1);
			zDesiredSteps = steps;
			z_steps_taken = 0;
			break;
		default:
			break;

	}
	stepperEN = 0;
	countSteps = 1;

}

void oneRevolution(uint16_t microstepping,int axis,int dir){

	uint8_t steps_per_revolution = 200;
	uint32_t steps =0;

	if(microstepping ==0){
		microstepping = 1;
	}

	steps = steps_per_revolution * microstepping;
	move_axis_stepper_motor(axis,steps,dir);

}

void stepper_direction(int axis,int dir){
    if(dir){  
        x_dir = 1;
        y_dir = 1;
        z_dir = 1;
    }
    else{    
        x_dir = 0;
        y_dir = 0;
        z_dir = 0;
    }
    return;
}

void enableSteppers(int axis, bool state){

    if(state){
        switch(axis){
            case 1:
                x_stepper.rise(&x_stepISR);
                x_step.resume();
                break;
            case 2:
                // y_stepper.rise(&y_stepISR);
                // y_step.resume();
                break;
            case 3:
                // z_stepper.rise(&z_stepISR);
                // z_step.resume();
                break;
            default:
                x_stepper.rise(&x_stepISR);
                // y_stepper.rise(&y_stepISR);
                // z_stepper.rise(&z_stepISR);
                x_step.resume();
                // y_step.resume();
                // z_step.resume();
        }      
    }
    else{
        switch(axis){
            case 1:
                x_step.suspend();
                x_stepper.rise(NULL);
                break;
            case 2:
                // y_step.suspend();
                // y_stepper.rise(NULL);
                break;
            case 3:
                // z_step.suspend();
                // z_stepper.rise(NULL);
                break;
            default:
                x_step.suspend();
                // y_step.suspend();
                // z_step.suspend();
                x_stepper.rise(NULL);
                // y_stepper.rise(NULL);
                // z_stepper.rise(NULL);
        }   
    }
}

void coordinatedMotion(int32_t steps, uint16_t target_velocity){

    if(steps<0){
        stepper_direction(1, 0);
    }
    else{
        stepper_direction(1, 1);
    }

    accel_d = ((float(target_velocity) * float(target_velocity)) / (2*max_acceleration));
    accel_t = float(target_velocity) / max_acceleration;

    constVelocity_d = abs(steps) - (2 * accel_d);

    constVelocity_t = float(constVelocity_d) / max_acceleration;

    freq_step = float(target_velocity-1600)/float(accel_d);

    xDesiredSteps = accel_d;
    x_steps_taken = 0;
    motion_phase = acceleration;

    enableSteppers(1,1);

	
	stepperEN = 0;
    countSteps = 1;

}

void constantAcceleration(uint32_t steps){

}

void timerCallbackX(){
	if(countSteps){
        x_steps_taken++;
        switch (motion_phase) {
            case acceleration:
                // Increment step count for axis Z
                if (x_steps_taken >= accel_d) {
                    x_step.period(1.0/(max_velocity));
                    motion_phase = constantV;
                }
                else{
                    x_step.period(1.0/(1600 + (x_steps_taken * freq_step)));
                }
            case constantV:
                if (x_steps_taken >= (accel_d+constVelocity_d)) {
                    motion_phase = deceleration;
                }
            case deceleration:
                if (x_steps_taken >= (2*accel_d+constVelocity_d)) {
                    x_step.suspend();
                    stepperEN = 1;
                    countSteps = 0;
                    x_steps_taken = 0;
                    xDesiredSteps = 0;
                }
                else{
                    x_step.period(1.0/(1600 + ((accel_d-(x_steps_taken-(accel_d+constVelocity_d))) * freq_step)));
                }
            default: 
                break;

        }
    }
}


void timerCallbackY(){
	if(countSteps & (xDesiredSteps != 0)){
        switch (motion_phase) {
            case acceleration:
                // Increment step count for axis Z
                x_steps_taken++;
                x_step.period(1.0/(freq_step + (x_steps_taken * freq_step)));
                x_step.write(0.5);
                if (x_steps_taken >= xDesiredSteps) {
                    x_steps_taken = 0;
                    xDesiredSteps = constVelocity_d;
                    motion_phase = constantV;
                }
            case constantV:
                x_steps_taken++;
                if (x_steps_taken >= xDesiredSteps) {
                    x_steps_taken = 0;
                    xDesiredSteps = accel_d;
                    motion_phase = deceleration;
                }
            case deceleration:
                x_steps_taken++;
                x_step.period(1.0/(freq_step + (((accel_d-1)-x_steps_taken) * freq_step)));
                x_step.write(0.5);
                if (x_steps_taken >= xDesiredSteps) {
                    stepperEN = 1;
                    countSteps = 0;
                    x_steps_taken = 0;
                    xDesiredSteps = 0;
                }
            default: 
                break;

        }
    }
}


void timerCallbackZ(){
	if(countSteps & (xDesiredSteps != 0)){
        switch (motion_phase) {
            case acceleration:
                // Increment step count for axis Z
                x_steps_taken++;
                x_step.period(1.0/(freq_step + (x_steps_taken * freq_step)));
                x_step.write(0.5);
                if (x_steps_taken >= xDesiredSteps) {
                    x_steps_taken = 0;
                    xDesiredSteps = constVelocity_d;
                    motion_phase = constantV;
                }
            case constantV:
                x_steps_taken++;
                if (x_steps_taken >= xDesiredSteps) {
                    x_steps_taken = 0;
                    xDesiredSteps = accel_d;
                    motion_phase = deceleration;
                }
            case deceleration:
                x_steps_taken++;
                x_step.period(1.0/(freq_step + (((accel_d-1)-x_steps_taken) * freq_step)));
                x_step.write(0.5);
                if (x_steps_taken >= xDesiredSteps) {
                    stepperEN = 1;
                    countSteps = 0;
                    x_steps_taken = 0;
                    xDesiredSteps = 0;
                }
            default: 
                break;

        }
    }
}

void x_stepISR(){
    motorQueueX.call(timerCallbackX);
}

void y_stepISR(){
    motorQueueY.call(timerCallbackY);
}

void z_stepISR(){
    motorQueueZ.call(timerCallbackZ);
}

