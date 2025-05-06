// Author: Samuel Theodore Gunawan, Jorel Alexander Setiabudi, Evan Aditya Chandra, Calvin Willyanto, Ignatius Timothy Bonario Simbolon
// SU ID: 2022390010, 2022390016, 2022390002, 2022390009, 2022390013
// Date: ...
// Microprossesor Final Project: "Race Yourself"

#include <Arduino.h>
#include <avr/io.h>
#include <string.h>
#include "timer.h"
#include "switch.h"
#include "lcd.h"
#include "infrared.h"
#include "ultrasonic.h"

// TODO: TO BE IMPLEMENTED
#include "led.h" 
#include "buzzer.h"

#define NUM_LEDS 144     // Number of LEDs on the strip

typedef enum {
    WAIT_PRESS,
    DB_PRESS,
    WAIT_RELEASE,
    DB_RELEASE
} state;

volatile state buttonState = WAIT_PRESS;


/*
 * Function to initialize all the components
 * For easier debugging and readability, we decided to separate all the init functions
 */ 
void initEverything() {
    initTimer1();
    initTimer0();
    initSwitchPB3();
    initLCD();
    initLED();
    initIR();
    UART_init(103);         // 16MHz, 9600 baud
    initUltrasonic();

    Serial.begin(9600);
    sei(); // Enable global interrupts
}

// Function for default display of LCD
void defaultDisplay() {
    // Clears the display
    clearDisplay();

    // Idle display (waiting for track to start)
    moveCursor(0, 0);                      // First line
    writeString("Race Yourself");
    moveCursor(0, 1);                      // Second line
    writeString("Idle...");
}

int main() {
    initEverything();
    defaultDisplay();

    char buf[32];

    while (1) {

        /* THIS IS FOR ULTRASONIC MEASURING AND PRINTING TO TERMINAL */
        // uint16_t dist = measureDistanceCm();

        // snprintf(buf, sizeof(buf), "Distance: %u cm\r\n", dist);
        // UART_SendString(buf);

        // delayMs(200); // 5 times per second


        /* THIS IS FOR LED STRIP WITHOUT FUNCTION (TESTING PURPOSES) */
        // for (int i = 0; i < NUM_LEDS; i++) {
        //     int j;
        //     for(j = 0; j < i; j++) {
        //         sendColor(0x00, 0x00, 0x00); // Green
        //     }
        //     sendColor(0x00, 0x01, 0x00); // Green
        //     for(j; j < NUM_LEDS; j++) {
        //         sendColor(0x00, 0x00, 0x00); // Green
        //     }
        //
        //     delayMs(500);
        // }


        /* TRY THIS FOR LED (SHOULD HAVE A CHASE ANIMATION LIKE ABOVE^) */
        // for (int j = 0; j < NUM_LEDS; j++) {
        //     lightLEDs(3, 4, j + 1); // Light up 4 LEDs in cyan color
        //     delayMs(500); // Delay for 100ms
        // }  


        /* IF THAT DOESN'T WORK, TRY THIS */
        // for (int j = 0; j < NUM_LEDS - 4; j++) {
        //     lightLEDs(3, 4, j + 1); // Light up 4 LEDs in cyan color
        //     delayMs(500); // Delay for 100ms
        // }


        /* TRY THIS TO LIGHT THE WHOLE STRIP UP (BLINKING ANIMATION, 5x) */
        // for (int blink = 0; blink < 5; blink++) {
        //     lightLEDs(4, NUM_LEDS, 1); // Light up all LEDs in red color
        //     delayMs(500); // Delay for 100ms
        //     lightLEDs(0, NUM_LEDS, 1); // Turn off all LEDs
        //     delayMs(500); // Delay for 100ms
        // }
    }
}

ISR(PCINT0_vect) {
    switch (buttonState) {
        case WAIT_PRESS:
            delayMs(30);
            buttonState = DB_PRESS;
  
        case DB_PRESS:
            delayMs(30);                        // Debounce delay
            cli();                              // Temporarily disables global interrupt to prevent multiple button press while countdown is ongoing.
            Serial.println("start button triggered");
        
            // TODO: START COUNTDOWN, BEGIN TRACKING

            buttonState = WAIT_RELEASE;         // Move to the next state after debouncing
            sei();                              // Re-activates global interrupts after finished
  
        case WAIT_RELEASE:
            delayMs(30);
            buttonState = DB_RELEASE;
        
        case DB_RELEASE:
            delayMs(30);
            buttonState = WAIT_PRESS;           // Return to initial state
    
        default:
            break;
    }
  }
