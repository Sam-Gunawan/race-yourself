#include "switch.h"

/*
 * Initializes pull-up resistor on PB3 and sets it into input mode
 */
void initSwitchPB3(){
    PORTB |= (1 << PB3); // init port B3 as pull up resistor
    DDRB &= ~(1 << DDB3); // set the pin 50 as input
  
    PCICR |= (1 << PCIE0); // enable interruption from this pin group
    PCMSK0 |= (1 << PCINT3); // enable interrupt specifically at pin PB3
}