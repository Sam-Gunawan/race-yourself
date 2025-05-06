#include "infrared.h"

void initIR() {
    DDRG &= ~(1 << PG5);   // Set PD4 as input (IR sensor pin)
    PORTG |= (1 << PG5);   // Enable pull-up resistor (optional for stability)
}

// Read IR sensor value
// Returns 0 if object is detected (LOW signal)
// Returns 1 if no object (HIGH signal)
uint8_t IRSensorRead() {
    return (PING & (1 << PG5)) ? 1 : 0; // Read the state of PD4 (IR sensor)
}