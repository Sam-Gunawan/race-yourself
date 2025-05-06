#ifndef ULTRASONIC_H
#define ULTRASONIC_H

#define F_CPU 16000000UL
#include <avr/io.h>
#include <stdio.h>


// Initializes the ultrasonic sensor.
void initUltrasonic();

// Measures and returns the distance in centimeters using the ultrasonic sensor.
uint16_t measureDistanceCm(int number);

#endif  