#include "limits.hpp"


#include "steppers.hpp"

InterruptIn xlimit1(xlimit1_PIN);
InterruptIn xlimit2(xlimit2_PIN);

InterruptIn y1limit1(y1limit1_PIN);
InterruptIn y1limit2(y1limit2_PIN);

InterruptIn y2limit1(y2limit1_PIN);
InterruptIn y2limit2(y2limit2_PIN);

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
    y2limit1.fall(y2limit1ISR);
    y2limit2.fall(y2limit2ISR);
    zlimit1.fall(zlimit1ISR);
    zlimit2.fall(zlimit2ISR);
}





void xlimit1ISR(){

    GPIOB->BSRR = (1U<<9);
    GPIOA->BSRR = (1U<<(4+16));
    
}

void xlimit2ISR(){

    GPIOB->BSRR = (1U<<9);
    GPIOA->BSRR = (1U<<(4+16));
    
}

void y1limit1ISR(){

    GPIOB->BSRR = (1U<<9);
    GPIOA->BSRR = (1U<<(4+16));
    
}

void y1limit2ISR(){

    GPIOB->BSRR = (1U<<9);
    GPIOA->BSRR = (1U<<(4+16));
    
}

void y2limit1ISR(){

    GPIOB->BSRR = (1U<<9);
    GPIOA->BSRR = (1U<<(4+16));
    
}

void y2limit2ISR(){

    GPIOB->BSRR = (1U<<9);
    GPIOA->BSRR = (1U<<(4+16));
    
}

void zlimit1ISR(){

    GPIOB->BSRR = (1U<<9);
    GPIOA->BSRR = (1U<<(4+16));
    
}

void zlimit2ISR(){

    GPIOB->BSRR = (1U<<9);
    GPIOA->BSRR = (1U<<(4+16));
    
}