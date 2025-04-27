#include "timer.h"

/* Initialize timer 1. Use CTC mode  .*/
void initTimer1(){
    // set to CTC mode
    TCCR1A &= ~(1 << WGM10);
    TCCR1A &= ~(1 << WGM11);
    TCCR1B |= (1 << WGM12);
    TCCR1B &= ~(1 << WGM13);

    // set prescalar to 8
    TCCR1B |= (1 << CS01) | (1 << CS00);
    TCCR1B &= ~(1 << CS02);
}

/* This delays the program an amount of microseconds specified by unsigned int delay. */
void delayUs(unsigned int delay){
    int prescaler = 8;
    OCR1A = ((0.000001 * 16000000) / prescaler) - 1; // calculate the value to be put in the OCR1A register

    for (unsigned int i = 0; i < delay; i++) {
        // set the timer to 0
        TCNT1 = 0;

        // wait until the OCF1A bit in the TIFR1 register is set
        // if the bit is set, it means that the timer has reached the value in OCR1A
        while (!(TIFR1 & (1 << OCF1A)));

        // clear compare match so that the next compare match can be detected
        TIFR1 |= (1 << OCF1A);
    }
}

void initTimer0(){
    // set to CTC mode
    TCCR0A &= ~(1 << WGM00);
    TCCR0A |= (1 << WGM11);
    TCCR0B &= ~(1 << WGM22);

    // set prescalar to 64
    TCCR0B |= (1 << CS01) | (1 << CS00);
    TCCR0B &= ~(1 << CS02);
}

/* This delays the program an amount specified by unsigned int delay.
* Use timer 0. Keep in mind that you need to choose your prescalar wisely
* such that your timer is precise to 1 millisecond and can be used for
* 100-200 milliseconds
*/
void delayMs(unsigned int delay){
    int prescaler = 64; 

    OCR0A = ((0.001 * 16000000) / prescaler) - 1; // calculate the value to be put in the OCR0A register

    for (unsigned int i = 0; i < delay; i++) {
        // set the timer to 0
        TCNT0 = 0;

        // wait until the OCF0A bit in the TIFR0 register is set
        // if the bit is set, it means that the timer has reached the value in OCR0A
        while (!(TIFR0 & (1 << OCF0A)));

        // clear compare match so that the next compare match can be detected
        TIFR0 |= (1 << OCF0A);
    }   
}