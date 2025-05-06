#ifndef ULTRASONIC_H
#define ULTRASONIC_H

//#define F_CPU 16000000UL
#include <avr/io.h>
#include <stdio.h>

// Initializes the UART communication with the given baud rate.
void UART_init(uint16_t ubrr);

// Sends a single character over UART.
void UART_SendChar(char c);

// Sends a null-terminated string over UART.
void UART_SendString(const char* s);

// Initializes the ultrasonic sensor.
void initUltrasonic();

// Measures and returns the distance in centimeters using the ultrasonic sensor.
uint16_t measureDistanceCm();

#endif