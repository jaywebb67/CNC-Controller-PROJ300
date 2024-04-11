#include "probe.hpp"


InterruptIn probe(probe_PIN);

bool probeTriggered = 0;

void probeISR();

void probeInit(){
    probeTriggered = 0;
    probe.rise(probeISR);

}

// Function to simulate probing operation
bool probeTool() {

    if(probeTriggered){
        return 1;
    }
    return 0;
}

void probeISR(){
    probeTriggered = 1;
}