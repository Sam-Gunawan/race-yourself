#ifndef LED_H
#define LED_H

#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>

void initLED();
inline void sendNops(uint8_t n);
void sendBit(uint8_t bit);
void sendByte(uint8_t byte);
void sendColor(uint8_t red, uint8_t green, uint8_t blue);
void sendLeds(uint8_t r, uint8_t g, uint8_t b, int count);
void lightLEDs(uint8_t r, uint8_t g, uint8_t b, int width, int start);

#endif