#include "jogButtons.hpp"


Thread jogThread;
EventQueue jogbuttonQueue;

InterruptIn xjog_plus(xJog_plus_pin);
InterruptIn xjog_minus(zJog_minus_pin);

InterruptIn yjog_plus(yJog_plus_pin);
InterruptIn yjog_minus(yJog_minus_pin);

InterruptIn zjog_plus(zJog_plus_pin);
InterruptIn zjog_minus(zJog_minus_pin);

void xplusHandler();
void xminusHandler();

void yplusHandler();
void yminusHandler();

void zplusHandler();
void zminusHandler();

void jogInit(){

    jogThread.start(callback(&jogbuttonQueue, &EventQueue::dispatch_forever));
    
}

void jogEnable(bool state){

    if(state){
        xjog_plus.rise(xplusHandler);
        xjog_minus.rise(xminusHandler);

        yjog_plus.rise(yplusHandler);
        yjog_minus.rise(yminusHandler);

        zjog_plus.rise(zplusHandler);
        zjog_minus.rise(zminusHandler);
    }
    else{
        xjog_plus.rise(NULL);
        xjog_minus.rise(NULL);

        yjog_plus.rise(NULL);
        yjog_minus.rise(NULL);

        zjog_plus.rise(NULL);
        zjog_minus.rise(NULL);
    }

}

void jogCallback(int axis, int dir){

    stepperInfoLock.lock();
    volatile stepperInfo& si = steppers[axis - 1];
    si.stepCount = 0;
    si.dir = dir;
    stepperInfoLock.unlock();

    GPIOB->BSRR = (1U << (9+16));
    switch(axis){
        case 1:
            stepperInfoLock.lock();
            si.dirFunc(dir);
            while(si.stepCount<=steps_per_increment){
                Xstep_HIGH
                wait_us(1500);
                Xstep_LOW
                wait_us(1500);
                stepperInfoLock.lock();
                si.stepCount++;
                si.stepPosition+=si.dir;
                stepperInfoLock.unlock();
            }
            stepperInfoLock.unlock();
            GPIOB->BSRR = (1U << 9);
            break;
        case 2:
            stepperInfoLock.lock();
            si.dirFunc(dir);
            while(si.stepCount<=steps_per_increment){
                Ystep_HIGH
                wait_us(1500);
                Ystep_LOW
                wait_us(1500);
                stepperInfoLock.lock();
                si.stepCount++;
                si.stepPosition+=si.dir;
                stepperInfoLock.unlock();
            }
            stepperInfoLock.unlock();
            GPIOB->BSRR = (1U << 9);
            break;
        case 3:
            stepperInfoLock.lock();
            si.dirFunc(dir);
            while(si.stepCount<=steps_per_increment){
                Zstep_HIGH
                wait_us(1500);
                Zstep_LOW
                wait_us(1500);
                stepperInfoLock.lock();
                si.stepCount++;
                si.stepPosition+=si.dir;
                stepperInfoLock.unlock();
            }
            stepperInfoLock.unlock();
            GPIOB->BSRR = (1U << 9);
            break;
        default:
            break;
    }
}

void xplusHandler(){
    wait_us(switchBounce_delayms * 1000);
    jogbuttonQueue.call(jogCallback,1,1);
}

void xminusHandler(){
    wait_us(switchBounce_delayms * 1000);
    jogbuttonQueue.call(jogCallback,1,0);
}

void yplusHandler(){
    wait_us(switchBounce_delayms * 1000);
    jogbuttonQueue.call(jogCallback,2,1);
}

void yminusHandler(){
    wait_us(switchBounce_delayms * 1000);
    jogbuttonQueue.call(jogCallback,2,0);
}

void zplusHandler(){
    wait_us(switchBounce_delayms * 1000);
    jogbuttonQueue.call(jogCallback,3,1);
}

void zminusHandler(){
    wait_us(switchBounce_delayms * 1000);
    jogbuttonQueue.call(jogCallback,3,0);
}