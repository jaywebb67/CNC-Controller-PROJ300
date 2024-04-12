#include "steppers.hpp"
#include <chrono>
#include <cstdint>
#include "limits.hpp"
#include "probe.hpp"
#include "peripherals.hpp"

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
volatile stepperInfo steppers[N_AXIS];
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
        GPIOB->BSRR = (1U << (4+16));
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
        GPIOB->BSRR = (1U << (3+16));
    }
}

void zStep() {
  Zstep_HIGH
  Zstep_LOW
}

void zDir(int dir){
    if(dir){
        GPIOB->BSRR = (1U << 6);
    }
    else{
        GPIOB->BSRR = (1U << (6+16));
    }
}

void stepperInit(){

    steppers[X_AXIS].dirFunc = xDir;
    steppers[X_AXIS].stepFunc = xStep;
    steppers[X_AXIS].acceleration = settings.acceleration[X_AXIS];
    steppers[X_AXIS].minStepInterval = settings.max_rate[X_AXIS];
    //steppers[0].stepCount = 0;

    steppers[Y_AXIS].dirFunc = yDir;
    steppers[Y_AXIS].stepFunc = yStep;
    steppers[Y_AXIS].acceleration = settings.acceleration[Y_AXIS];
    steppers[Y_AXIS].minStepInterval = settings.max_rate[Y_AXIS];
    //steppers[1].stepCount = 0;

    steppers[Z_AXIS].dirFunc = zDir;
    steppers[Z_AXIS].stepFunc = zStep;
    steppers[Z_AXIS].acceleration = settings.acceleration[Z_AXIS];
    steppers[Z_AXIS].minStepInterval = settings.max_rate[Z_AXIS];
    //steppers[2].stepCount = 0;

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

void prepareMovement(int whichMotor, float target_distance) {
    volatile stepperInfo& si = steppers[whichMotor];
    long targetSteps = (target_distance*settings.steps_per_mm[whichMotor]);
    long steps = targetSteps - si.stepPosition;
    printf("Target steps: %ld\r\n",targetSteps);
    printf("Step count: %ld\n\r",si.stepPosition);
    printf("Steps: %ld\r\n",steps);
    if(steps == 0){
        return;
    }
    
    si.dirFunc( steps > 0 ? 1 : 0 );
    si.dir = steps > 0 ? 1 : -1;
    si.totalSteps = abs(steps);
    resetStepper(si);

    remainingSteppersFlag |= (1 << (whichMotor));

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
  
  for (int i = 0; i < N_AXIS; i++) {
    if ( ! ((1 << i) & remainingSteppersFlag) )
      continue;
    if ( steppers[i].estTimeForMove > maxTime )
      maxTime = steppers[i].estTimeForMove;
  }

  if ( maxTime != 0 ) {
    for (int i = 0; i < N_AXIS; i++) {
      if ( ! ( (1 << i) & remainingSteppersFlag) )
        continue;
      steppers[i].speedScale = maxTime / steppers[i].estTimeForMove;
    }
  }
}

void calculateAccelerationArray(){
    for (int i = 0; i < N_AXIS; i++){
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
            while(holdFlag){};
            Xstep_HIGH
            wait_us(x.accel_di[i]);
            Xstep_LOW
            x.stepCount++;
            x.stepPosition += x.dir;
            //wait_us(x.accel_di[i]);
        }
        for (int i = 0; i<(x.totalSteps - 2*(x.rampUpStepCount+1)); i++){
            while(holdFlag){};
            Xstep_HIGH
            wait_us(x.minStepInterval*x.speedScale);
            Xstep_LOW
            x.stepCount++;
            x.stepPosition += x.dir;
            
        }
        for (int i = x.rampUpStepCount;i>=0;i--){
            while(holdFlag){};
            Xstep_HIGH
            wait_us(x.decel_di[i]);
            Xstep_LOW
            x.stepCount++;
            x.stepPosition += x.dir;
            if ( x.stepCount >= x.totalSteps ) {
                //printf("%d X steps decel\n\r",x.stepCount);
                x.movementDone = true;
                remainingSteppersFlag &= ~(1 << 0);
                if (!remainingSteppersFlag) {
                    stepperEN = 1;
                    motorFlag.clear(1);
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
            while(holdFlag){};
            Ystep_HIGH
            wait_us(y.accel_di[i]);
            Ystep_LOW
            y.stepCount++;
            y.stepPosition += y.dir;
        }
        for (int i = 0; i<(y.totalSteps - 2*(y.rampUpStepCount+1)); i++){
            while(holdFlag){};
            Ystep_HIGH
            wait_us(y.minStepInterval*y.speedScale);
            Ystep_LOW
            y.stepCount++;
            y.stepPosition += y.dir;
        }
        for (int i = y.rampUpStepCount;i>=0;i--){
            while(holdFlag){};
            Ystep_HIGH
            wait_us(y.decel_di[i]);
            Ystep_LOW
            y.stepCount++;
            y.stepPosition += y.dir;
            if ( y.stepCount >= y.totalSteps ) {
                //printf("%d Y steps decel\n\r",y.stepCount);
                y.movementDone = true;
                remainingSteppersFlag &= ~(1 << 1);
                if (!remainingSteppersFlag) {
                    stepperEN = 1;
                    motorFlag.clear(1);

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
            while(holdFlag){};
            Zstep_HIGH
            wait_us(z.accel_di[i]);
            Zstep_LOW
            z.stepCount++;
            z.stepPosition += z.dir;
        }
        for (int i = 0; i<(z.totalSteps - 2*(z.rampUpStepCount+1)); i++){
            while(holdFlag){};
            Zstep_HIGH
            wait_us(z.minStepInterval*z.speedScale);
            Zstep_LOW
            z.stepCount++;
            z.stepPosition += z.dir;
        }
        for (int i = z.rampUpStepCount;i>=0;i--){
            while(holdFlag){};
            Zstep_HIGH
            wait_us(z.decel_di[i]);
            Zstep_LOW
            z.stepCount++;
            z.stepPosition += z.dir;
            if ( z.stepCount >= z.totalSteps ) {
                //printf("%d Z steps decel\n\r",z.stepCount);
                z.movementDone = true;
                remainingSteppersFlag &= ~(1 << 2);
                if (!remainingSteppersFlag) {
                    stepperEN = 1;
                    motorFlag.clear(1);

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


void motionArc(float *position, float *target, float *offset, float radius, float feed_rate,
            uint8_t invert_feed_rate, uint8_t axis_0, uint8_t axis_1, uint8_t axis_linear, uint8_t is_clockwise_arc){

    float center_axis0 = position[axis_0] + offset[axis_0];
    float center_axis1 = position[axis_1] + offset[axis_1];
    float r_axis0 = -offset[axis_0];  // Radius vector from center to current location
    float r_axis1 = -offset[axis_1];
    float rt_axis0 = target[axis_0] - center_axis0;
    float rt_axis1 = target[axis_1] - center_axis1;

    // CCW angle between position and target from circle center. Only one atan2() trig computation required.
    float angular_travel = atan2(r_axis0*rt_axis1-r_axis1*rt_axis0, r_axis0*rt_axis0+r_axis1*rt_axis1);
    if (is_clockwise_arc) { // Correct atan2 output per direction
        if (angular_travel >= -ARC_ANGULAR_TRAVEL_EPSILON) { 
            angular_travel -= 2*M_PI; 
        }
    } 
    else {
        if (angular_travel <= ARC_ANGULAR_TRAVEL_EPSILON) { 
            angular_travel += 2*M_PI; 
        }
    }

    // NOTE: Segment end points are on the arc, which can lead to the arc diameter being smaller by up to
    // (2x) settings.arc_tolerance. For 99% of users, this is just fine. If a different arc segment fit
    // is desired, i.e. least-squares, midpoint on arc, just change the mm_per_arc_segment calculation.
    // For the intended uses of Grbl, this value shouldn't exceed 2000 for the strictest of cases.
    uint16_t segments = floor(fabs(0.5*angular_travel*radius)/
                            sqrt(/*settings.arcTolerance*/0.02*(2*radius - 0.02/*settings.arc_tolerance*/)) );

    if (segments) { 
        // Multiply inverse feed_rate to compensate for the fact that this movement is approximated
        // by a number of discrete segments. The inverse feed_rate should be correct for the sum of 
        // all segments.
        if (invert_feed_rate) { 
            feed_rate *= segments;
        }

        float theta_per_segment = angular_travel/segments;
        float linear_per_segment = (target[axis_linear] - position[axis_linear])/segments;

        /* Vector rotation by transformation matrix: r is the original vector, r_T is the rotated vector,
            and phi is the angle of rotation. Solution approach by Jens Geisler.
                r_T = [cos(phi) -sin(phi);
                        sin(phi)  cos(phi] * r ;
            
            For arc generation, the center of the circle is the axis of rotation and the radius vector is 
            defined from the circle center to the initial position. Each line segment is formed by successive
            vector rotations. Single precision values can accumulate error greater than tool precision in rare
            cases. So, exact arc path correction is implemented. This approach avoids the problem of too many very
            expensive trig operations [sin(),cos(),tan()] which can take 100-200 usec each to compute.

            Small angle approximation may be used to reduce computation overhead further. A third-order approximation
            (second order sin() has too much error) holds for most, if not, all CNC applications. Note that this 
            approximation will begin to accumulate a numerical drift error when theta_per_segment is greater than 
            ~0.25 rad(14 deg) AND the approximation is successively used without correction several dozen times. This
            scenario is extremely unlikely, since segment lengths and theta_per_segment are automatically generated
            and scaled by the arc tolerance setting. Only a very large arc tolerance setting, unrealistic for CNC 
            applications, would cause this numerical drift error. However, it is best to set N_ARC_CORRECTION from a
            low of ~4 to a high of ~20 or so to avoid trig operations while keeping arc generation accurate.
            
            This approximation also allows mc_arc to immediately insert a line segment into the planner 
            without the initial overhead of computing cos() or sin(). By the time the arc needs to be applied
            a correction, the planner should have caught up to the lag caused by the initial mc_arc overhead. 
            This is important when there are successive arc motions. 
        */
        // Computes: cos_T = 1 - theta_per_segment^2/2, sin_T = theta_per_segment - theta_per_segment^3/6) in ~52usec
        float cos_T = 2.0 - theta_per_segment*theta_per_segment;
        float sin_T = theta_per_segment*0.16666667*(cos_T + 4.0);
        cos_T *= 0.5;

        float sin_Ti;
        float cos_Ti;
        float r_axisi;
        uint16_t i;
        uint8_t count = 0;

        for (i = 1; i<segments; i++) { // Increment (segments-1).
            
            if (count < N_ARC_CORRECTION) {
                // Apply vector rotation matrix. ~40 usec
                r_axisi = r_axis0*sin_T + r_axis1*cos_T;
                r_axis0 = r_axis0*cos_T - r_axis1*sin_T;
                r_axis1 = r_axisi;
                count++;
            } 
            else {      
                // Arc correction to radius vector. Computed only every N_ARC_CORRECTION increments. ~375 usec
                // Compute exact location by applying transformation matrix from initial radius vector(=-offset).
                cos_Ti = cos(i*theta_per_segment);
                sin_Ti = sin(i*theta_per_segment);
                r_axis0 = -offset[axis_0]*cos_Ti + offset[axis_1]*sin_Ti;
                r_axis1 = -offset[axis_0]*sin_Ti - offset[axis_1]*cos_Ti;
                count = 0;
            }

            // Update arc_target location
            position[axis_0] = center_axis0 + r_axis0;
            position[axis_1] = center_axis1 + r_axis1;
            position[axis_linear] += linear_per_segment;
            
            prepareMovement(axis_0, position[axis_0]);
            prepareMovement(axis_1, position[axis_1]);
            prepareMovement(axis_linear, position[axis_linear]);
            runAndWait();

        }
    }
    // Ensure last segment arrives at target location.
    prepareMovement(axis_0, target[axis_0]);
    prepareMovement(axis_1, target[axis_1]);
    prepareMovement(axis_linear, target[axis_linear]);
    runAndWait();
    return;


}    

void homing_cycle(){
        // Check and abort homing cycle, if hard limits are already enabled. Helps prevent problems
    // with machines with limits wired on both ends of travel to one limit pin.
    // TODO: Move the pin-specific LIMIT_PIN call to limits.c as a function.
    if (limits_get_state()) { 
        
        system_reset();
    }


    limitsEnable(0); // Disable hard limits pin change register for cycle duration

    // Search to engage all axes limit switches at faster homing seek rate.
    limitsHOME(HOMING_CYCLE_0);  // Homing cycle 0
    #ifdef HOMING_CYCLE_1
        limitsHOME(HOMING_CYCLE_1);  // Homing cycle 1
    #endif
    #ifdef HOMING_CYCLE_2
        limitsHOME(HOMING_CYCLE_2);  // Homing cycle 2
    #endif

    if (sys.abort) { 
        return; 
    } // Did not complete. Alarm state set by mc_alarm.

    // Homing cycle complete! Setup system for normal operation.
    // -------------------------------------------------------------------------------------

    // Gcode parser position was circumvented by the limits_go_home() routine, so sync position now.
    for (uint8_t i =0;i<N_AXIS;i++){
        steppers[i].stepPosition = 0;
    }

    // If hard limits feature enabled, re-enable hard limits pin change register after homing cycle.
    limitsEnable(1);
    return;

}

void probeCycle(float *target, float feed_rate, uint8_t invert_feed_rate){

    // Initialize probing control variables
    sys.probe_succeeded = false; // Re-initialize probe history before beginning cycle.  

    if (sys.abort) { 
        return; 
    } // Return if system reset has been issued.

    // Setup and queue probing motion. Auto cycle-start should not start the cycle.
    prepareMovement(X_AXIS, target[X_AXIS]);
    prepareMovement(Y_AXIS, target[Y_AXIS]);
    prepareMovement(Z_AXIS, target[Z_AXIS]);
    runAndWait();

    volatile stepperInfo& zStepper = steppers[Z_AXIS];
    stepperEN = 0;
    while(!probeTool()){

        while(holdFlag){};
        Zstep_HIGH
        wait_us(750);
        Zstep_LOW
        wait_us(750);
        zStepper.stepPosition++;

    }
    
    //home Z-axis
    prepareMovement(X_AXIS, target[X_AXIS]);
    prepareMovement(Y_AXIS, target[Y_AXIS]);
    prepareMovement(Z_AXIS, 0);
    runAndWait();

    //re-cycle to check that
    uint32_t testProbeDist = 0.0;
    while(!probeTool()){
        while(holdFlag){};
        Zstep_HIGH
        wait_us(325);
        Zstep_LOW
        wait_us(325);
        testProbeDist++;
    }
    if(testProbeDist == zStepper.stepPosition){
        sys.probe_succeeded = true;
        line_state.tool_length_offset = (settings.max_travel[Z_AXIS] - (zStepper.stepPosition*settings.steps_per_mm[Z_AXIS]));
    }
    homing_cycle();
    return;
}