#define F_CPU 16000000UL
#include <Arduino.h>  // Include Arduino library for Serial support
#include <util/delay.h>
#include <stdio.h>
#include "ultrasonic.h"
#include "timer.h"

// ==== Pin Definitions ====
#define TRIG_PIN  PE4  // Digital Pin 
#define ECHO_PIN  PE5  // Digital Pin 
// #define TRIG_PIN1  PE4  // Digital Pin 
// #define ECHO_PIN1  PE5  // Digital Pin 
// #define TRIG_PIN2  PE0  // Digital Pin 
// #define ECHO_PIN2  PE1  // Digital Pin 

// ==== HC-SR04 Setup ====
void initUltrasonic() {
    DDRE |= (1 << TRIG_PIN);    // TRIG as output
    DDRE &= ~(1 << ECHO_PIN);   // ECHO as input
    // DDRE |= (1 << TRIG_PIN1);    // TRIG as output
    // DDRE &= ~(1 << ECHO_PIN1);   // ECHO as input
    // DDRE |= (1 << TRIG_PIN2);    // TRIG as output for the 2nd sensor
    // DDRE &= ~(1 << ECHO_PIN2); 
}

uint16_t measureDistanceCm(int number) {
    uint32_t count = 0;
    // uint8_t TRIG_PIN;
    // uint8_t ECHO_PIN;

    // if (number == 1){
    //     TRIG_PIN = TRIG_PIN1;
    //     ECHO_PIN = ECHO_PIN1;
    // } else if (number == 2){
    //     TRIG_PIN = TRIG_PIN2;
    //     ECHO_PIN = ECHO_PIN2;
    // }
    // Trigger 10us pulse
    PORTE &= ~(1 << TRIG_PIN);
    _delay_us(2);
    PORTE |= (1 << TRIG_PIN);
    _delay_us(10);
    PORTE &= ~(1 << TRIG_PIN);

    // Wait for ECHO HIGH (timeout)
    uint32_t timeout = 60000;
    while (!(PINE & (1 << ECHO_PIN)) && timeout--) _delay_us(1);
    if (timeout == 0) return 0;

    // Measure ECHO HIGH pulse duration
    count = 0;
    while ((PINE & (1 << ECHO_PIN))) {
        _delay_us(1);
        count++;
        if (count > 30000) break; // timeout ~500 cm
    }

    return count / 58;  // convert to cm
}