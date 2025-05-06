#ifndef IR_H
#define IR_H

#include <avr/io.h>

void initIR();
uint8_t IRSensorRead(uint8_t number);

#endif