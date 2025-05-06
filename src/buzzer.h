#ifndef BUZZER_H
#define BUZZER_H

#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>

void setupPWM();
void playNote(uint16_t frequency);
void delay_ms(uint16_t ms);


#endif