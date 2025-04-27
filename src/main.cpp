// Author: Samuel Theodore Gunawan, Jorel Alexander Setiabudi, Evan Aditya Chandra, Calvin Willyanto, Ignatius Timothy Bonario Simbolon
// SU ID: 2022390010
// Date: ...
// Microprossesor Final Project: "Race Yourself"

#include <Arduino.h>
#include <avr/io.h>
#include <string.h>
#include "timer.h"
#include "switch.h"
#include "lcd.h"

// TODO: TO BE IMPLEMENTED
#include "led.h" 
#include "ultrasonic.h"
#include "buzzer.h"

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

    // TODO: INIT ULTRASONIC, LED STRIP, BUZZER

    Serial.begin(9600);
    sei(); // Enable global interrupts
}

/*
 * Function for default display of LCD
 */
void defaultDisplay() {
    // Clears the display
    moveCursor(0, 1);                      // Moves the cursor to (0, 1) position (first line)
    writeString("                ");
    moveCursor(0, 0);                      // Moves the cursor to (0, 0) position (second line)
    writeString("                ");

    // Idle display (waiting for track to start)
    moveCursor(0, 1);                      // First line
    writeString("Race Yourself");
    moveCursor(0, 0);                      // Second line
    writeString("Idle...");
}

void main() {
    initEverything();
    defaultDisplay();

    while (1) {
        // TODO: LED ANIMATION WHILE IDLE
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