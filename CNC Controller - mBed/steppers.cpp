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

Mutex stepperInfoLock;

Thread motorMovementX(osPriorityRealtime);
// EventQueue motorQueueX;

Thread motorMovementY(osPriorityRealtime);
// EventQueue motorQueueY;

Thread motorMovementZ(osPriorityRealtime);
// EventQueue motorQueueZ;

volatile stepperInfo steppers[stepperNumber];
volatile uint8_t remainingSteppersFlag = 0;



void motorXThread();
void timerCallbackX();

void motorYThread();
void timerCallbackY();

void motorZThread();
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


    motorMovementX.start(motorXThread);
    motorMovementY.start(motorYThread);
    motorMovementZ.start(motorZThread);
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


void motorXThread(){
    stepperInfoLock.lock();
    volatile stepperInfo& s = steppers[0];
    stepperInfoLock.unlock();
    // unsigned long currentDelay = s.di;
    while(true){
        ThisThread::flags_wait_any(1);
        xTimer.detach();
        if(remainingSteppersFlag & (1U << 0)){
            //currentDelay = s.di;
            stepperInfoLock.lock();
            if ( s.stepCount < s.totalSteps ) {
                s.stepFunc();
                s.stepCount++;
                s.stepPosition += s.dir;
                if ( s.stepCount >= s.totalSteps ) {
                    s.movementDone = true;
                    stepperInfoLock.unlock();
                    ThisThread::flags_clear(1);
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
            xTimer.attach(&timerCallbackX,s.di*1us);
            stepperInfoLock.unlock();
            ThisThread::flags_clear(1);
        }
        else{
            disableStepperInterrupt(1);
            ThisThread::flags_clear(1);
            remainingSteppersFlag &= ~(1 << 0);
            return;
        }
    }
}

void motorYThread(){
    stepperInfoLock.lock();
    volatile stepperInfo& s = steppers[1];
    stepperInfoLock.unlock();
    while(true){
        ThisThread::flags_wait_any(2);
        yTimer.detach();
        if(remainingSteppersFlag & (1U << 1)){
            stepperInfoLock.lock();
            if ( s.stepCount < s.totalSteps ) {
                s.stepFunc();
                s.stepCount++;
                s.stepPosition += s.dir;
                if ( s.stepCount >= s.totalSteps ) {
                    s.movementDone = true;
                    stepperInfoLock.unlock();
                    ThisThread::flags_clear(2);
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
            yTimer.attach(&timerCallbackY,s.di*1us);
            stepperInfoLock.unlock();
            ThisThread::flags_clear(2);
        }
        else{
            disableStepperInterrupt(2);
            ThisThread::flags_clear(2);
            remainingSteppersFlag &= ~(1 << 1);
            return;
        }
    }
}

void motorZThread(){
    stepperInfoLock.lock();
    volatile stepperInfo& s = steppers[2];
    stepperInfoLock.unlock();
    while(true){
        ThisThread::flags_wait_any(4);
        zTimer.detach();
        if(remainingSteppersFlag & (1U << 2)){
            stepperInfoLock.lock();
            if ( s.stepCount < s.totalSteps ) {
                s.stepFunc();
                s.stepCount++;
                s.stepPosition += s.dir;
                if ( s.stepCount >= s.totalSteps ) {
                    s.movementDone = true;
                    stepperInfoLock.unlock();
                    ThisThread::flags_clear(4);
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
            zTimer.attach(&timerCallbackZ,s.di*1us);
            stepperInfoLock.unlock();
            ThisThread::flags_clear(4);
        }
        else{
            disableStepperInterrupt(3);
            ThisThread::flags_clear(4);
            remainingSteppersFlag &= ~(1 << 2);
            return;
        }
    }
}

void timerCallbackX(){
    motorMovementX.flags_set(1);
}

void timerCallbackY(){
    motorMovementY.flags_set(2);
}

void timerCallbackZ(){
    motorMovementY.flags_set(4);
}