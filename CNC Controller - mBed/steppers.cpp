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

Thread motorThreadX(osPriorityRealtime);
EventQueue motorQueueX;

Thread motorThreadY(osPriorityRealtime);
EventQueue motorQueueY;

Thread motorThreadZ(osPriorityRealtime);
EventQueue motorQueueZ;

Thread printThread;
EventQueue printQueue;

EventFlags motorFlag;
volatile stepperInfo steppers[stepperNumber];
volatile uint8_t remainingSteppersFlag = 0;


void motorXMovement();


void motorYMovement();

void motorZMovement();

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


    motorThreadX.start(callback(&motorQueueX, &EventQueue::dispatch_forever));
    motorThreadY.start(callback(&motorQueueY, &EventQueue::dispatch_forever));
    motorThreadZ.start(callback(&motorQueueZ, &EventQueue::dispatch_forever));
    printThread.start(callback(&printQueue, &EventQueue::dispatch_forever));
}

void resetStepper(volatile stepperInfo& si) {
    si.c0 = si.acceleration;
    si.d = si.c0;
    si.accel_di[0] = si.d;
    si.decel_di[0] = si.d;
    si.stepCount = 0;
    si.n = 0;
    si.rampUpStepCount = 0;
    si.movementDone = false;
    si.speedScale = 1;

    float a = si.minStepInterval / (float)si.c0;
    a *= 0.676;

    float m = ((a*a - 1) / (-2 * a));
    float n = m * m;

    si.estStepsToSpeed = ceil(n);
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

void calculateAccelerationArray(){
    for (int i = 0; i < stepperNumber; i++){
        volatile stepperInfo& s = steppers[i];
        if(!((1U<<i)&remainingSteppersFlag)){
            continue;
        }
        s.rampUpStepTime += s.accel_di[0];
        while ( s.rampUpStepCount == 0 ) {
            s.n++;
            s.d = s.d - (2 * s.d) / (4 * s.n + 1);
            s.accel_di[s.n] = s.d * s.speedScale; // integer
            if ( s.d <= s.minStepInterval ) {
                s.d = s.minStepInterval;
                s.rampUpStepCount = s.n;
            }
            if ( s.n >= s.totalSteps / 2 ) {
                s.rampUpStepCount = s.n;
            }
            s.rampUpStepTime += s.d;
        }

        for(int j = s.n; j>=1;j--){
            s.d = (s.d * (4 * j + 1)) / (4 * i + j - 2);
            if ( s.d >= s.c0 ) {
                s.d = s.c0;
            }
            s.decel_di[j] = s.d * s.speedScale;
        }
    }
}

void runAndWait() {
    stepperEN = 0;
    adjustSpeedScales();
    calculateAccelerationArray();
    motorQueueX.call(motorXMovement);
    motorQueueY.call(motorYMovement);
    motorQueueZ.call(motorZMovement);
    wait_us(100);
    motorFlag.set(1);
    while(remainingSteppersFlag){};
    return;
}


void motorXMovement(){
    stepperInfoLock.lock();
    volatile stepperInfo& x = steppers[0];
    stepperInfoLock.unlock();

    if(remainingSteppersFlag & (1U << 0)){
        //currentDelay = s.di;
        if(!motorFlag.wait_all_for(1, 1s,0)){
            printQueue.call(printf,"Error motor flag not set for motor operation! System restarting...");
            system_reset();
        }
        for (int i = 0; i<=x.rampUpStepCount; i++){
            Xstep_HIGH
            Xstep_LOW
            x.stepCount++;
            x.stepPosition += x.dir;
            wait_us(x.accel_di[i]);
        }
        for (int i = 0; i<(x.totalSteps - 2*(x.rampUpStepCount+1)); i++){
            Xstep_HIGH
            Xstep_LOW
            x.stepCount++;
            x.stepPosition += x.dir;
            wait_us(x.minStepInterval*x.speedScale);
        }
        for (int i = x.rampUpStepCount;i>=0;i--){
            Xstep_HIGH
            Xstep_LOW
            x.stepCount++;
            x.stepPosition += x.dir;
            wait_us(x.decel_di[i]);
            if ( x.stepCount >= x.totalSteps ) {
                printf("%d X steps decel\n\r",x.stepCount);
                x.movementDone = true;
                remainingSteppersFlag &= ~(1 << 0);
                if (!remainingSteppersFlag) {
                    motorFlag.clear(1);
                    stepperEN = 1;
                    return;
                }
            }
        }
    }
    else{
        remainingSteppersFlag &= ~(1 << 0);
        return;
    }
}

void motorYMovement(){
    stepperInfoLock.lock();
    volatile stepperInfo& y = steppers[1];
    stepperInfoLock.unlock();

    if(remainingSteppersFlag & (1U << 1)){
        //currentDelay = s.di;
        if(!motorFlag.wait_all_for(1, 1s,0)){
            printQueue.call(printf,"Error motor flag not set for motor operation! System restarting...");
            system_reset();
        }
        for (int i = 0; i<=y.rampUpStepCount; i++){
            Ystep_HIGH
            Ystep_LOW
            y.stepCount++;
            y.stepPosition += y.dir;
            wait_us(y.accel_di[i]);
        }
        for (int i = 0; i<(y.totalSteps - 2*(y.rampUpStepCount+1)); i++){
            Ystep_HIGH
            Ystep_LOW
            y.stepCount++;
            y.stepPosition += y.dir;
            wait_us(y.minStepInterval*y.speedScale);
        }
        for (int i = y.rampUpStepCount;i>=0;i--){
            Ystep_HIGH
            Ystep_LOW
            y.stepCount++;
            y.stepPosition += y.dir;
            wait_us(y.decel_di[i]);
            if ( y.stepCount >= y.totalSteps ) {
                printf("%d Y steps decel\n\r",y.stepCount);
                y.movementDone = true;
                remainingSteppersFlag &= ~(1 << 1);
                if (!remainingSteppersFlag) {
                    motorFlag.clear(1);
                    stepperEN = 1;
                    return;
                }
            }
        }
    }
    else{
        remainingSteppersFlag &= ~(1 << 1);
        return;
    }
}

void motorZMovement(){
    stepperInfoLock.lock();
    volatile stepperInfo& z = steppers[2];
    stepperInfoLock.unlock();

    if(remainingSteppersFlag & (1U << 2)){
        //currentDelay = s.di;
        if(!motorFlag.wait_all_for(1, 1s,0)){
            printQueue.call(printf,"Error motor flag not set for motor operation! System restarting...");
            system_reset();
        }
        for (int i = 0; i<=z.rampUpStepCount; i++){
            Zstep_HIGH
            Zstep_LOW
            z.stepCount++;
            z.stepPosition += z.dir;
            wait_us(z.accel_di[i]);
        }
        for (int i = 0; i<(z.totalSteps - 2*(z.rampUpStepCount+1)); i++){
            Zstep_HIGH
            Zstep_LOW
            z.stepCount++;
            z.stepPosition += z.dir;
            wait_us(z.minStepInterval*z.speedScale);
        }
        for (int i = z.rampUpStepCount;i>=0;i--){
            Zstep_HIGH
            Zstep_LOW
            z.stepCount++;
            z.stepPosition += z.dir;
            wait_us(z.decel_di[i]);
            if ( z.stepCount >= z.totalSteps ) {
                printf("%d Z steps decel\n\r",z.stepCount);
                z.movementDone = true;
                remainingSteppersFlag &= ~(1 << 2);
                if (!remainingSteppersFlag) {
                    motorFlag.clear(1);
                    stepperEN = 1;
                    return;
                }
            }
        }
    }
    else{
        remainingSteppersFlag &= ~(1 << 2);
        return;
    }
}
