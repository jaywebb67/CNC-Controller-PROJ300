#include "jogButtons.hpp"
#include "steppers.hpp"

Thread jogThread;
EventQueue jogbuttonQueue;

InterruptIn xjog_plus(xJog_plus_pin);
InterruptIn xjog_minus(zJog_minus_pin);

// InterruptIn yjog_plus(yJog_plus_pin);
// InterruptIn yjog_minus(yJog_minus_pin);

// InterruptIn zjog_plus(zJog_plus_pin);
// InterruptIn zjog_minus(zJog_minus_pin);

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

        // yjog_plus.rise(yplusHandler);
        // yjog_minus.rise(yminusHandler);

        // zjog_plus.rise(zplusHandler);
        // zjog_minus.rise(zminusHandler);
    }
    else{
        xjog_plus.rise(NULL);
        xjog_minus.rise(NULL);

        // yjog_plus.rise(NULL);
        // yjog_minus.rise(NULL);

        // zjog_plus.rise(NULL);
        // zjog_minus.rise(NULL);
    }

}

void jogCallback(int axis, int dir){
    jogEnable(0);
    int i = 0;
    uint32_t steps = 4;
    GPIOB->ODR &= ~(1U << 9);
    switch(axis){
        case 1:
            GPIOB->ODR |= (dir << 4);
            while(i<=steps){
                GPIOB->ODR |= (1U << 15);
                wait_us(5000);
                GPIOB->ODR &= ~(1U << 15);
                wait_us(5000);
                i++;
            }
            GPIOB->ODR |= (1U << 9);
            break;
        case 2:
            GPIOB->ODR |= (dir << 3);
            for(int i = 0;i<=steps;i++){
                GPIOB->ODR |= (1U << 8);
                wait_us(625);
                GPIOB->ODR &= ~(1U << 8);
                wait_us(625);
            }
            break;
        case 3:
            GPIOB->ODR |= (dir << 6);
            for(int i = 0;i<=steps;i++){
                GPIOB->ODR |= (1U << 5);
                wait_us(625);
                GPIOB->ODR &= ~(1U << 5);
                wait_us(625);
            }
            break;
    }
    jogEnable(1);
}

void xplusHandler(){
    jogbuttonQueue.call(jogCallback,1,1);
}

void xminusHandler(){
    jogbuttonQueue.call(jogCallback,1,0);
}

void yplusHandler(){
    jogbuttonQueue.call(jogCallback,2,1);
}

void yminusHandler(){
    jogbuttonQueue.call(jogCallback,2,0);
}

void zplusHandler(){
    jogbuttonQueue.call(jogCallback,3,1);
}

void zminusHandler(){
    jogbuttonQueue.call(jogCallback,3,0);
}