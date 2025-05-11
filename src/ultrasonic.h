#ifndef ULTRASONIC_H
#define ULTRASONIC_H

#define F_CPU 16000000UL
#include <avr/io.h>
#include <stdio.h>


// Initializes the ultrasonic sensor.
void initUltrasonic();

// Measures and returns the distance in centimeters using the ultrasonic sensor.
uint16_t measureDistanceCm(int number);

void UART_init(uint16_t ubrr);
void UART_SendChar(char c);
void UART_SendString(const char* s);

#endif  