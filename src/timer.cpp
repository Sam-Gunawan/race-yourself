#include "timer.h"

/* Initialize timer 1. Use CTC mode  .*/
void initTimer1(){
    // Set to CTC mode
    TCCR1A &= ~(1 << WGM10);
    TCCR1A &= ~(1 << WGM11);
    TCCR1B |= (1 << WGM12);
    TCCR1B &= ~(1 << WGM13);

    // Set prescalar to 8
    TCCR1B |= (1 << CS01) | (1 << CS00);
    TCCR1B &= ~(1 << CS02);
}

/* This delays the program an amount of microseconds specified by unsigned int delay. */
void delayUs(unsigned int delay){
    int prescaler = 8;
    OCR1A = ((0.000001 * 16000000) / prescaler) - 1; // Calculate the value to be put in the OCR1A register

    for (unsigned int i = 0; i < delay; i++) {
        // Set the timer to 0
        TCNT1 = 0;

        // Wait until the OCF1A bit in the TIFR1 register is set
        // If the bit is set, it means that the timer has reached the value in OCR1A
        while (!(TIFR1 & (1 << OCF1A)));

        // Clear compare match so that the next compare match can be detected
        TIFR1 |= (1 << OCF1A);
    }
}

void delayMs(unsigned int delay){
    for (unsigned int i = 0; i < delay; i++) {
        delayUs(1000);    // Delay 1 milisecond for each delay
    }
}

/* Initialize timer 0. Use CTC mode  .*/
void initTimer0(){
    // set to CTC mode
    TCCR0A &= ~(1 << WGM00);
    TCCR0A |= (1 << WGM11);
    TCCR0B &= ~(1 << WGM22);

    // set prescalar to 64
    TCCR0B |= (1 << CS01) | (1 << CS00);
    TCCR0B &= ~(1 << CS02);
}

/* Returns the time since arduino started in microseconds. Works like micros() in Arduino library */
unsigned long microsSinceStart() {
    unsigned long time;

    // Disable interrupts to avoid interrupt conflicts
    cli();
    
    // Read the Timer0 value, which counts microseconds.
    time = TCNT0; 

    // Re-enable interrupts
    sei();

    // Timer0 ticks every 4 clock cycles (prescaler = 64, 16MHz / 64 = 250kHz, 1 tick = 1 / 250kHz = 4 us)
    return time / 4;  // In this case we divide by 4 to get the time per 1 microsecond
}