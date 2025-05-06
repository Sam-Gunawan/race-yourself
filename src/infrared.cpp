#include "infrared.h"
// #define OUTPUT_PIN1  PK0  // Digital Pin 
// #define OUTPUT_PIN1  PK1  // Digital Pin 
// #define OUTPUT_PIN1  PK2  // Digital Pin 


void initIR() {
    DDRK &= ~((1 << PK0) | (1 << PK1) | (1 << PK2));  // Set PK0–PK2 as inputs
    PORTK |= (1 << PK0) | (1 << PK1) | (1 << PK2);    // Enable pull-up resistors
    
    // Enable PCINTs for PK0, PK1, PK2 (PCINT0, PCINT1, PCINT2)
    PCICR |= (1 << PCIE2); // Enable PCINT[7:0] group (Port A)
    PCMSK2 |= (1 << PCINT17) | (1 << PCINT18); // Enable specific pins

}

// Read IR sensor value
// Returns 0 if object is detected (LOW signal)
// Returns 1 if no object (HIGH signal)
uint8_t IRSensorRead(uint8_t number) {
    int8_t INFRA_PIN;
    if (number == 1) {
        INFRA_PIN = PK0;
    } else if (number == 2) {
        INFRA_PIN = PK1;
    } else {
        INFRA_PIN = PK2;
    }
    
    return (PINK & (1 << INFRA_PIN)) ? 1 : 0; // Read the state of PD4 (IR sensor)
}