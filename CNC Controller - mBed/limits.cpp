#include "limits.hpp"


#include "steppers.hpp"
#include <cstdint>
#include <cstdlib>
#include "system.hpp"
#include "peripherals.hpp"

using namespace std;

InterruptIn xlimit1(xlimit1_PIN);
InterruptIn xlimit2(xlimit2_PIN);

InterruptIn y1limit1(y1limit1_PIN);
InterruptIn y1limit2(y1limit2_PIN);

//InterruptIn y2limit1(y2limit1_PIN);
//InterruptIn y2limit2(y2limit2_PIN);

InterruptIn zlimit1(zlimit1_PIN);
InterruptIn zlimit2(zlimit2_PIN);

void xlimit1ISR();
void xlimit2ISR();
void y1limit1ISR();
void y1limit2ISR();
void y2limit1ISR();
void y2limit2ISR();
void zlimit1ISR();
void zlimit2ISR();



void limitsInit(){


    xlimit1.fall(xlimit1ISR);
    xlimit2.fall(xlimit2ISR);
    y1limit1.fall(y1limit1ISR);
    y1limit2.fall(y1limit2ISR);
    //y2limit1.fall(y2limit1ISR);
    //y2limit2.fall(y2limit2ISR);
    zlimit1.fall(zlimit1ISR);
    zlimit2.fall(zlimit2ISR);

}

void limitsEnable(bool EN){
    if (EN){
        xlimit1.fall(xlimit1ISR);
        xlimit2.fall(xlimit2ISR);
        y1limit1.fall(y1limit1ISR);
        y1limit2.fall(y1limit2ISR);
      //  y2limit1.fall(y2limit1ISR);
        //y2limit2.fall(y2limit2ISR);
        zlimit1.fall(zlimit1ISR);
        zlimit2.fall(zlimit2ISR);
    }
    else{
        xlimit1.fall(NULL);
        xlimit2.fall(NULL);
        y1limit1.fall(NULL);
        y1limit2.fall(NULL);
        //y2limit1.fall(NULL);
        //y2limit2.fall(NULL);
        zlimit1.fall(NULL);
        zlimit2.fall(NULL);
    }
}



uint8_t softLimitCheck(lineStruct_block_t line_structure){

    uint8_t limitsFlag = 0;

    if (line_structure.values.xyz[X_AXIS] >= settings.max_travel[X_AXIS]){
        printf("X-axis planned move exceeds axis limit\n\r");
        limitsFlag |= (1U << X_AXIS);
    }
    else if(line_structure.values.xyz[Y_AXIS] >= settings.max_travel[Y_AXIS]){
        printf("Y-axis planned move exceeds axis limit\n\r");
        limitsFlag |= (1U << Y_AXIS);
    }
    else if (line_structure.values.xyz[Z_AXIS] >= settings.max_travel[Z_AXIS]) {
        printf("Z-axis planned move exceeds axis limit\n\r");
        limitsFlag |= (1U << Z_AXIS);
    }
    
    return limitsFlag;
    

}

void limitPullOff(uint8_t axis){



    stepperInfoLock.lock();
    volatile stepperInfo& si = steppers[axis];
    si.stepCount = 0;
    stepperInfoLock.unlock();


    switch(axis){
        case X_AXIS:
            GPIOB->BSRR = (1U << (9+16));
            stepperInfoLock.lock();
            si.dir = !si.dir;
            si.dirFunc(si.dir);
            stepperInfoLock.unlock();
            while(si.stepCount<=400){
                while(holdFlag){};
                Xstep_HIGH
                wait_us(1500);
                Xstep_LOW
                wait_us(1500);
                stepperInfoLock.lock();
                si.stepCount++;
                si.stepPosition+=si.dir;
                stepperInfoLock.unlock();
            }
            GPIOB->BSRR = (1U << 9);    
            break;
        case Y_AXIS:
            GPIOB->BSRR = (1U << (9+16));
            stepperInfoLock.lock();
            si.dir = !si.dir;
            si.dirFunc(si.dir);
            stepperInfoLock.unlock();
            while(si.stepCount<=400){
                while(holdFlag){};
                Ystep_HIGH
                wait_us(1500);
                Ystep_LOW
                wait_us(1500);
                stepperInfoLock.lock();
                si.stepCount++;
                si.stepPosition+=si.dir;
                stepperInfoLock.unlock();
            }
            GPIOB->BSRR = (1U << 9);  
            break;
        case Z_AXIS:
            GPIOB->BSRR = (1U << (9+16));
            stepperInfoLock.lock();
            si.dir = !si.dir;
            si.dirFunc(si.dir);
            stepperInfoLock.unlock();
            while(si.stepCount<=400){
                while(holdFlag){};
                Ystep_HIGH
                wait_us(1500);
                Ystep_LOW
                wait_us(1500);
                stepperInfoLock.lock();
                si.stepCount++;
                si.stepPosition+=si.dir;
                stepperInfoLock.unlock();
            }
            GPIOB->BSRR = (1U << 9);  
            break;
        default:
            #ifdef DUAL_Y_AXIS
                if((limit == 3)&(axis == Y_AXIS) ){
                    y_dir = !y_dir;
                }
                if((limit == 4)&(axis == Y_AXIS) ){
                    y_dir = !y_dir;
                }
            #endif
            break;
    }
    printf("System resetting");
    system_reset();
}

uint8_t limits_get_state()
{
    uint8_t limit_state = 0;
    uint8_t pin = (LIMIT_PIN & LIMIT_MASK);
    #ifdef INVERT_LIMIT_PIN_MASK
        pin ^= INVERT_LIMIT_PIN_MASK;
    #endif
    if (bit_isfalse(settings.flags,BITFLAG_INVERT_LIMIT_PINS)) { 
        pin ^= LIMIT_MASK; 
    }
    if (pin) {  
        uint8_t idx;
        for (idx=0; idx<N_AXIS; idx++) {
            if (pin & get_limit_pin_mask(idx)) { 
                limit_state |= (1 << idx); 
            }
        }
    }
    return(limit_state);
}

bool limitSwitchTriggered(uint8_t axis){
    uint16_t limitPins = 0;
    switch(axis){
        case X_AXIS:
            limitPins = (bit(0)|bit(2));
            break;
        case Y_AXIS:
            limitPins = (bit(3)|bit(4));
            break;
        case Z_AXIS:
            limitPins = (bit(5)|bit(6));
            break;
    }
    if(GPIOE->IDR & limitPins){
        return 1;
    }
    else{
        return 0;
    }
}

void pullOffMotion(uint8_t axis){
    float steps = settings.homing_pulloff * settings.steps_per_mm[axis];
    stepperEN = 0;
    for (int i = 0;i<=steps;i++){
        while(holdFlag){};
        switch(axis){
            case X_AXIS:
                Xstep_HIGH
                wait_us(300);
                Xstep_LOW
                wait_us(300);
                break;
            case Y_AXIS:

                Ystep_HIGH
                wait_us(300);
                Ystep_LOW
                wait_us(300);
                break;
            case Z_AXIS:
                Zstep_HIGH
                wait_us(300);
                Zstep_LOW
                wait_us(300);
                break;
        }
    }
    stepperEN = 1;
    return;
}

void limitsHOME(uint8_t cycle_mask){
    // Initialize
    long target[N_AXIS] = {0};   
    float homing_rate = settings.homing_seek_rate;
    long steps[N_AXIS] = {0};
    long targetSteps[N_AXIS] = {0};


    for(int i=0;i<3;i++){
        //convert homing rate to delay microseconds
        homing_rate = (1/(homing_rate/60)*settings.steps_per_mm[i])*100000;
        if(homing_rate != steppers[i].minStepInterval){
            steppers[i].minStepInterval = homing_rate;
        }
        
    }
    stepperEN = 0;
    if(cycle_mask & bit(X_AXIS)){
        steps[X_AXIS] = target[X_AXIS] - steppers[X_AXIS].stepPosition;
        steppers[X_AXIS].dirFunc( steps[X_AXIS] >(long)0 ? 1 : 0 );
        steppers[X_AXIS].dir = steps[X_AXIS] > 0 ? 1 : -1;
        while (!limitSwitchTriggered(X_AXIS)) {
            while(holdFlag){};
            Xstep_HIGH
            wait_us(steppers[X_AXIS].minStepInterval);
            Xstep_LOW
            if (sys.abort) return; // Check for system reset
        }
        // Perform pull-off motion for Z-axis
        pullOffMotion(X_AXIS);
    }
    if(cycle_mask & bit(Y_AXIS)){
        steps[Y_AXIS] = target[Y_AXIS] - steppers[Y_AXIS].stepPosition;
        steppers[Y_AXIS].dirFunc( steps[Y_AXIS] >(long)0 ? 1 : 0 );
        steppers[Y_AXIS].dir = steps[Y_AXIS] > 0 ? 1 : -1;
        while (!limitSwitchTriggered(Y_AXIS)) {
            while(holdFlag){};
            Ystep_HIGH
            wait_us(steppers[Y_AXIS].minStepInterval);
            Ystep_LOW
            if (sys.abort) return; // Check for system reset
        }
        // Perform pull-off motion for Z-axis
        pullOffMotion(Y_AXIS);
    }
    if(cycle_mask & bit(Z_AXIS)){
        steps[Z_AXIS] = target[Z_AXIS] - steppers[Z_AXIS].stepPosition;
        steppers[Z_AXIS].dirFunc( steps[Z_AXIS] >(long)0 ? 1 : 0 );
        steppers[Z_AXIS].dir = steps[Z_AXIS] > 0 ? 1 : -1;
        //step motor until limit switch triggered
        while (!limitSwitchTriggered(Z_AXIS)) {
            while(holdFlag){};
            Zstep_HIGH
            wait_us(steppers[Z_AXIS].minStepInterval);
            Zstep_LOW
            if (sys.abort) return; // Check for system reset
        }
        // Perform pull-off motion for Z-axis
        pullOffMotion(Z_AXIS);
    }
    stepperEN = 1;

    for(uint8_t i=0;i<N_AXIS;i++){
        steppers[i].stepPosition = 0.0;
        line_state.position[i] = 0.0;
        sys.position[i] = 0.0;
    }
    return;
        
}



void xlimit1ISR(){

    GPIOB->BSRR = (1U<<9);
    //spindle fast stop
    spindleA = 0.0f; spindleB = 0.0f;
    //Clear spindle enable
    GPIOA->BSRR = (1U<<(4+16));
    //reset both pins atomically using the Bit Set/Reset Register
    GPIOF->BSRR = (1U<<(mistPin_No+16)|(1U<<(floodPin_No+16)));
    limitPullOff(X_AXIS);
}

void xlimit2ISR(){

    GPIOB->BSRR = (1U<<9);
    //spindle fast stop
    spindleA = 0.0f; spindleB = 0.0f;
    //Clear spindle enable
    GPIOA->BSRR = (1U<<(4+16));
    //reset both pins atomically using the Bit Set/Reset Register
    GPIOF->BSRR = (1U<<(mistPin_No+16)|(1U<<(floodPin_No+16)));
    limitPullOff(X_AXIS);
}

void y1limit1ISR(){

    GPIOB->BSRR = (1U<<9);

    //spindle fast stop
    spindleA = 0.0f; spindleB = 0.0f;
    //Clear spindle enable
    GPIOA->BSRR = (1U<<(4+16));
    //reset both pins atomically using the Bit Set/Reset Register
    GPIOF->BSRR = (1U<<(mistPin_No+16)|(1U<<(floodPin_No+16)));
    limitPullOff(Y_AXIS);
}

void y1limit2ISR(){

    GPIOB->BSRR = (1U<<9);
    //spindle fast stop
    spindleA = 0.0f; spindleB = 0.0f;
    //Clear spindle enable
    GPIOA->BSRR = (1U<<(4+16));
    //reset both pins atomically using the Bit Set/Reset Register
    GPIOF->BSRR = (1U<<(mistPin_No+16)|(1U<<(floodPin_No+16)));
    limitPullOff(Y_AXIS);
}

void y2limit1ISR(){

    GPIOB->BSRR = (1U<<9);
    //spindle fast stop
    spindleA = 0.0f; spindleB = 0.0f;
    //Clear spindle enable
    GPIOA->BSRR = (1U<<(4+16));
    //reset both pins atomically using the Bit Set/Reset Register
    GPIOF->BSRR = (1U<<(mistPin_No+16)|(1U<<(floodPin_No+16)));
    limitPullOff(Y_AXIS);
}

void y2limit2ISR(){

    GPIOB->BSRR = (1U<<9);
    //spindle fast stop
    spindleA = 0.0f; spindleB = 0.0f;
    //Clear spindle enable
    GPIOA->BSRR = (1U<<(4+16));
    //reset both pins atomically using the Bit Set/Reset Register
    GPIOF->BSRR = (1U<<(mistPin_No+16)|(1U<<(floodPin_No+16)));
    limitPullOff(Y_AXIS);
}

void zlimit1ISR(){

    GPIOB->BSRR = (1U<<9);
    //spindle fast stop
    spindleA = 0.0f; spindleB = 0.0f;
    //Clear spindle enable
    GPIOA->BSRR = (1U<<(4+16));
    //reset both pins atomically using the Bit Set/Reset Register
    GPIOF->BSRR = (1U<<(mistPin_No+16)|(1U<<(floodPin_No+16)));
    limitPullOff(Z_AXIS);
}

void zlimit2ISR(){
    
    GPIOB->BSRR = (1U<<9);
    //spindle fast stop
    spindleA = 0.0f; spindleB = 0.0f;
    //Clear spindle enable
    GPIOA->BSRR = (1U<<(4+16));
    //reset both pins atomically using the Bit Set/Reset Register
    GPIOF->BSRR = (1U<<(mistPin_No+16)|(1U<<(floodPin_No+16)));
    limitPullOff(Z_AXIS);
}