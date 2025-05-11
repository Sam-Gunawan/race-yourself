#include <avr/interrupt.h>
#include <avr/io.h>
#include <Arduino.h>
#include "spi.h"
#include "switch.h"
#include "timer.h"
#include "matrix.h"
#include "scrolltext.h"


uint8_t number3[8] = {
    0b00111100,
    0b01000010,
    0b00000010,
    0b00011100,
    0b00000010,
    0b00000010,
    0b01000010,
    0b00111100
};

uint8_t number2[8] = {
    0b00111100,
    0b01000010,
    0b00000010,
    0b00000100,
    0b00001000,
    0b00010000,
    0b00100000,
    0b01111110
};

uint8_t number1[8] = {
    0b00001000,
    0b00011000,
    0b00101000,
    0b00001000,
    0b00001000,
    0b00001000,
    0b00001000,
    0b01111110
};

uint8_t clear[8] = {
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000
};



void displayFace(uint8_t face[8]) {
    for (uint8_t i = 0; i < 8; i++) {
        MAX7219_Send(i + 1, face[i]);
    }
}

void displayCountDown() {
    displayFace(number3);
    delayMs(100);

    displayFace(clear);
    delayMs(10);

    displayFace(number2);
    delayMs(100);

    displayFace(clear);
    delayMs(10);

    displayFace(number1);
    delayMs(100);

    displayFace(clear);

    setMessage("GOOO  ");
    scrollMessage();
    
}

// ✅ Clean, callable countdown initializer
void StartCountdown() {
    // Initialize only once before displaying countdown
    SPI_MASTER_Init();
    MAX7219_Init();
  

    // Begin the countdown display
    displayCountDown();


}

/* ---- Deprecated/Old Code (Commented Out) ---- */

// enum stateEnum {wait_press, debounce_press, wait_release, debounce_release};
// enum stateEnum state = wait_press; // Initialize the state machine

// ISR(PCINT0_vect) {
//     if (state == wait_press) {
//         state = debounce_press;
//     } else if (state == wait_release) {
//         state = debounce_release;
//     }
// }
