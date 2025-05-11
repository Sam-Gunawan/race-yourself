#include "switch.h"

/*
 * Initializes pull-up resistor on PB3 and sets it into input mode
 */
void initSwitchPB3(){
    PORTB |= (1 << PB3); // init port B3 as pull up resistor
    DDRB &= ~(1 << DDB3); // set the pin 50 as input
  
    PCICR |= (1 << PCIE0); // enable interruption from this pin group
    PCMSK0 |= (1 << PCINT3); // enable interrupt specifically at pin PB3
    
    PORTB |= (1 << PB5); // init port B3 as pull up resistor
    DDRB &= ~(1 << DDB5); // set the pin 50 as input
    PCMSK0 |= (1 << PCINT5); // enable interrupt specifically at pin PB3

    // PORTE |= (1 << PE1); // init port B3 as pull up resistor
    // PORTJ |= (1 << PJ1);    // Enable pull-up resistors
    // DDRJ &= ~(1 << DDJ1); // set the pin 50 as input

  
    // PCICR |= (1 << PCIE1); // enable interruption from this pin group
    // PCMSK1 |= (1 << PCINT10); // enable interrupt specifically at pin PB3
    // DDRD &= ~(1 << DDD2);    // Set PD2 as input
    // PORTD |= (1 << PD2);     // Enable pull-up resistor on PD2

    // EICRA |= (1 << ISC21);   // Trigger on falling edge
    // EICRA &= ~(1 << ISC20);  // (ISC21 = 1, ISC20 = 0) => falling edge

    // EIMSK |= (1 << INT2);    // Enable external interrupt INT2
}