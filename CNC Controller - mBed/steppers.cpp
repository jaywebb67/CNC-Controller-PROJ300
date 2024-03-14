#include "steppers.hpp"
#include <chrono>
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


// Thread motorMovementX;
// EventQueue motorQueueX;

// Thread motorMovementY;
// EventQueue motorQueueY;

// Thread motorMovementZ;
// EventQueue motorQueueZ;

volatile stepperInfo steppers[stepperNumber];
volatile uint8_t remainingSteppersFlag = 0;



void timerCallbackX();
void timerCallbackY();
void timerCallbackZ();

void xStep() {
  Xstep_HIGH
  Xstep_LOW
}

void xDir(int dir){
    if(dir){
        GPIOB->BSRR = (1U << 4);
    }
    else{
        GPIOB->BSRR = (1U << (4+15));
    }
}

void yStep() {
  Ystep_HIGH
  Ystep_LOW
}

void yDir(int dir){
    if(dir){
        GPIOB->BSRR = (1U << 3);
    }
    else{
        GPIOB->BSRR = (1U << (3+15));
    }
}

void zStep() {
  Zstep_HIGH
  Zstep_LOW
}

void zDir(int dir){
    if(dir){
        GPIOB->BSRR = (1U << 5);
    }
    else{
        GPIOB->BSRR = (1U << (5+15));
    }
}

void stepperInit(volatile int accel,volatile int max_speed){

    steppers[0].dirFunc = xDir;
    steppers[0].stepFunc = xStep;
    steppers[0].acceleration = accel;
    steppers[0].minStepInterval = max_speed;

    steppers[1].dirFunc = yDir;
    steppers[1].stepFunc = yStep;
    steppers[1].acceleration = accel;
    steppers[1].minStepInterval = max_speed;

    steppers[2].dirFunc = zDir;
    steppers[2].stepFunc = zStep;
    steppers[2].acceleration = accel;
    steppers[2].minStepInterval = max_speed;


    stepperEN = 1;


    // motorMovementX.start(callback(&motorQueueX, &EventQueue::dispatch_forever));
    // motorMovementY.start(callback(&motorQueueY, &EventQueue::dispatch_forever));
    // motorMovementZ.start(callback(&motorQueueZ, &EventQueue::dispatch_forever));
}

void resetStepper(volatile stepperInfo& si) {
    si.c0 = si.acceleration;
    si.d = si.c0;
    si.di = si.d;
    si.stepCount = 0;
    si.n = 0;
    si.rampUpStepCount = 0;
    si.movementDone = false;
    si.speedScale = 1;

    float a = si.minStepInterval / (float)si.c0;
    a *= 0.676;

    float m = ((a*a - 1) / (-2 * a));
    float n = m * m;

    si.estStepsToSpeed = n;
}


float getDurationOfAcceleration(volatile stepperInfo& s, unsigned int numSteps) {
  float d = s.c0;
  float totalDuration = 0;
  for (unsigned int n = 1; n < numSteps; n++) {
    d = d - (2 * d) / (4 * n + 1);
    totalDuration += d;
  }
  return totalDuration;
}

void prepareMovement(int whichMotor, long steps) {
  volatile stepperInfo& si = steppers[whichMotor-1];
  si.dirFunc( steps > 0 ? 1 : 0 );
  si.dir = steps > 0 ? 1 : -1;
  si.totalSteps = abs(steps);
  resetStepper(si);
  
  remainingSteppersFlag |= (1 << (whichMotor-1));

  unsigned long stepsAbs = abs(steps);

  if ( (2 * si.estStepsToSpeed) < stepsAbs ) {
    // there will be a period of time at full speed
    unsigned long stepsAtFullSpeed = stepsAbs - 2 * si.estStepsToSpeed;
    float accelDecelTime = getDurationOfAcceleration(si, si.estStepsToSpeed);
    si.estTimeForMove = 2 * accelDecelTime + stepsAtFullSpeed * si.minStepInterval;
  }
  else {
    // will not reach full speed before needing to slow down again
    float accelDecelTime = getDurationOfAcceleration( si, stepsAbs / 2 );
    si.estTimeForMove = 2 * accelDecelTime;
  }
}

void adjustSpeedScales() {
  float maxTime = 0;
  
  for (int i = 0; i < stepperNumber; i++) {
    if ( ! ((1 << i) & remainingSteppersFlag) )
      continue;
    if ( steppers[i].estTimeForMove > maxTime )
      maxTime = steppers[i].estTimeForMove;
  }

  if ( maxTime != 0 ) {
    for (int i = 0; i < stepperNumber; i++) {
      if ( ! ( (1 << i) & remainingSteppersFlag) )
        continue;
      steppers[i].speedScale = maxTime / steppers[i].estTimeForMove;
    }
  }
}


void runAndWait() {
    stepperEN = 0;
    adjustSpeedScales();
    xTimer.attach(&timerCallbackX, steppers[0].di*1us);
    yTimer.attach(&timerCallbackY, steppers[1].di*1us);
    zTimer.attach(&timerCallbackZ, steppers[2].di*1us); 
    while(remainingSteppersFlag){};
    return;
}

void disableStepperInterrupt(int axis){

    switch(axis){
        case 1:
            xTimer.detach();
            break;
        case 2:
            yTimer.detach();
            break;
        case 3:
            zTimer.detach();
            break;
        default:
            xTimer.detach();
            yTimer.detach();
            zTimer.detach();
            break;
    }  
    return; 
}


void timerCallbackX(){
    if(remainingSteppersFlag & (1U << 0)){
        volatile stepperInfo& s = steppers[0];
        unsigned long currentDelay = s.di;
        if ( s.stepCount < s.totalSteps ) {
            s.stepFunc();
            s.stepCount++;
            s.stepPosition += s.dir;
            if ( s.stepCount >= s.totalSteps ) {
                s.movementDone = true;
                remainingSteppersFlag &= ~(1 << 0);
                if (!remainingSteppersFlag) {
                    stepperEN = 1;
                    return;
                }
            }
        }

        if ( s.rampUpStepCount == 0 ) {
            s.n++;
            s.d = s.d - (2 * s.d) / (4 * s.n + 1);
            if ( s.d <= s.minStepInterval ) {
            s.d = s.minStepInterval;
            s.rampUpStepCount = s.stepCount;
            }
            if ( s.stepCount >= s.totalSteps / 2 ) {
            s.rampUpStepCount = s.stepCount;
            }
            s.rampUpStepTime += s.d;
        }
        else if ( s.stepCount >= s.totalSteps - s.rampUpStepCount ) {
            s.d = (s.d * (4 * s.n + 1)) / (4 * s.n + 1 - 2);
            s.n--;
        }

        s.di = s.d * s.speedScale; // integer

        //std::chrono::microseconds delay = std::chrono::microseconds(s.di) - (std::chrono::microseconds(currentDelay) - (xTimer.remaining_time()));;
        
        xTimer.attach(&timerCallbackX,s.di*1us);
    }
    else{
        disableStepperInterrupt(1);
        remainingSteppersFlag &= ~(1 << 0);
        return;
    }
}


void timerCallbackY(){
    if(remainingSteppersFlag & (1U << 1)){
        volatile stepperInfo& s = steppers[1];
        unsigned long currentDelay = s.di;
        if ( s.stepCount < s.totalSteps ) {
            s.stepFunc();
            s.stepCount++;
            s.stepPosition += s.dir;
            if ( s.stepCount >= s.totalSteps ) {
                s.movementDone = true;
                remainingSteppersFlag &= ~(1 << 1);
                if (!remainingSteppersFlag) {
                    stepperEN = 1;
                    return;
                }
            }
        }

        if ( s.rampUpStepCount == 0 ) {
            s.n++;
            s.d = s.d - (2 * s.d) / (4 * s.n + 1);
            if ( s.d <= s.minStepInterval ) {
            s.d = s.minStepInterval;
            s.rampUpStepCount = s.stepCount;
            }
            if ( s.stepCount >= s.totalSteps / 2 ) {
            s.rampUpStepCount = s.stepCount;
            }
            s.rampUpStepTime += s.d;
        }
        else if ( s.stepCount >= s.totalSteps - s.rampUpStepCount ) {
            s.d = (s.d * (4 * s.n + 1)) / (4 * s.n + 1 - 2);
            s.n--;
        }

        s.di = s.d * s.speedScale; // integer

        //std::chrono::microseconds delay = std::chrono::microseconds(s.di) - (std::chrono::microseconds(currentDelay) - (xTimer.remaining_time()));;
        
        yTimer.attach(&timerCallbackY,s.di*1us);
    }
    else{
        disableStepperInterrupt(2);
        remainingSteppersFlag &= ~(1 << 1);
        return;
    }
}

void timerCallbackZ(){
    if(remainingSteppersFlag & (1U << 2)){
        volatile stepperInfo& s = steppers[2];
        unsigned long currentDelay = s.di;
        if ( s.stepCount < s.totalSteps ) {
            s.stepFunc();
            s.stepCount++;
            s.stepPosition += s.dir;
            if ( s.stepCount >= s.totalSteps ) {
                s.movementDone = true;
                remainingSteppersFlag &= ~(1 << 2);
                if (!remainingSteppersFlag) {
                    stepperEN = 1;
                    return;
                }
            }
        }

        if ( s.rampUpStepCount == 0 ) {
            s.n++;
            s.d = s.d - (2 * s.d) / (4 * s.n + 1);
            if ( s.d <= s.minStepInterval ) {
            s.d = s.minStepInterval;
            s.rampUpStepCount = s.stepCount;
            }
            if ( s.stepCount >= s.totalSteps / 2 ) {
            s.rampUpStepCount = s.stepCount;
            }
            s.rampUpStepTime += s.d;
        }
        else if ( s.stepCount >= s.totalSteps - s.rampUpStepCount ) {
            s.d = (s.d * (4 * s.n + 1)) / (4 * s.n + 1 - 2);
            s.n--;
        }

        s.di = s.d * s.speedScale; // integer

        //std::chrono::microseconds delay = std::chrono::microseconds(s.di) - (std::chrono::microseconds(currentDelay) - (xTimer.remaining_time()));;
        
        zTimer.attach(&timerCallbackZ,s.di*1us);
    }
    else{
        disableStepperInterrupt(3);
        remainingSteppersFlag &= ~(1 << 2);
        return;
    }
}

// void x_stepISR(){
//     motorQueueX.call(timerCallbackX);
// }

// void y_stepISR(){
//     motorQueueY.call(timerCallbackY);
// }

// void z_stepISR(){
//     motorQueueZ.call(timerCallbackZ);
// }

