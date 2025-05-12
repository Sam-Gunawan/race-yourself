#include <avr/interrupt.h>
#include <avr/io.h>
#include <Arduino.h>
#include "spi.h"
#include "switch.h"
#include "timer.h"
#include "matrix.h"
#include "scrolltext.h"


uint8_t number3[8] = {
    0b00000000,
    0b01101110,
    0b10010001,
    0b10010001,
    0b10010001,
    0b10000010,
    0b01000100,
    0b00000000
};



uint8_t number2[8] = {
    0b00000000,
    0b01100001,
    0b10010001,
    0b10001001,
    0b10000101,
    0b10000011,
    0b01000001,
    0b00000000
};



uint8_t number1[8] = {
    0b00000001,
    0b00000001,
    0b11111111,
    0b01000001,
    0b00100001,
    0b00000001,
    0b00000001,
    0b00000000
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




