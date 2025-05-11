// Author: Samuel Theodore Gunawan, Jorel Alexander Setiabudi, Evan Aditya Chandra, Calvin Willyanto, Ignatius Timothy Bonario Simbolon
// SU ID: 2022390010, 2022390016, 2022390002, 2022390009, 2022390013
// Date: ...
// Microprossesor Final Project: "Race Yourself"

#include <Arduino.h>
#include <avr/io.h>
#include <string.h>
#include <util/delay.h>
#include "timer.h"
#include "switch.h"
#include "lcd.h"
#include "infrared.h"
#include "ultrasonic.h"
#include "matrix.h"
#include "buzzer.h"


// TODO: TO BE IMPLEMENTED
#include "led.h" 


#define NUM_LEDS 144     // Number of LEDs on the strip
#define DISTANCE 1      // The distance of the LED strip 

typedef enum {
    WAIT_PRESS,
    DB_PRESS,
    WAIT_RELEASE,
    DB_RELEASE
} state;


volatile state buttonState = WAIT_PRESS;

volatile bool raceStarted = false;
volatile bool raceFinished = false;
volatile bool isValid = true;
volatile unsigned int startTime;
volatile unsigned int finishTime;
volatile unsigned int raceDuration;
volatile bool mode = 0; // 0: track mode, 1: ghost mode
volatile bool ghostMode = false;
volatile double speed;

int INDEX_LEDS = 0;
volatile double counter = 0;
double raceTime;
double ghostTime = 0;




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
    setupPWM();
    initIR();
    // UART_init(103);         // 16MHz, 9600 baud
    initUltrasonic();

    Serial.begin(9600);
    sei(); // Enable global interrupts
}

// Function for default display of LCD
void defaultDisplay(bool mode) {
    // Clears the display
    clearDisplay();

    // Idle display (waiting for track to start)
    moveCursor(0, 0);                      // First line
    writeString("Race Yourself");
    moveCursor(0, 1);                      // Second line
    
    if (!mode) {writeString("Mode: Track");}
    else {writeString("Mode: Ghost");}
}

void raceFinishedSequence() {
    clearDisplay();

    moveCursor(0, 0);

}

void standby(bool mode) {
    defaultDisplay(mode);
    lightLEDs(0xFF, 0xFF, 0x00, NUM_LEDS, 1); // Light up all LEDs in yellow color
    delayMs(1);
}


int main() {
    initEverything();

    while (1) {
        initLED(); // Reinitialize the LED strip
        // Serial.print("Racestart: ");
        // Serial.println(raceStarted);
        // Serial.print("Racefinish: ");
        // Serial.println(raceFinished);
        // Serial.print("isvalid: ");
        // Serial.println(isValid);
        // delayMs(100);
        if (raceStarted == false){raceFinished = false;}
        

        if (raceStarted) {
            initLED(); // Reinitialize the LED strip

            if (raceFinished) {
                initLED(); // Reinitialize the LED strip
                finishTime = microsSinceStart();
                Serial.println("Finish time: ");
                Serial.println(finishTime);

                raceDuration = finishTime - startTime;

                Serial.print("Race time (using microsSinceStart()): ");
                Serial.println(raceDuration);

                Serial.print("Race time (using counter): ");

                raceTime = counter/9.17; // Convert to seconds
                Serial.println(raceTime);
                
                if (raceTime > ghostTime) {ghostTime = raceTime;} // Save fastest time record to ghost time
                
                speed = DISTANCE / raceTime; // Calculate the speed m/s
                
                Serial.print("Run time speed: ");
                Serial.println(speed);
                char raceTimeSpeed[16];
                sprintf(raceTimeSpeed, "%.2f", speed); // Keep 2 decimal place for the speed display


                clearDisplay();
                moveCursor(0, 0);
                writeString("Race Finished!");
                moveCursor(0, 1);
                char raceTimeString[16];
                sprintf(raceTimeString, "%.2f", raceTime);  // Keep 2 decimal places
                writeString(raceTimeString);


                counter = 0;
                raceDuration = 0;
                startTime = 0;
                raceFinished = false; // Reset raceFinished
                raceStarted = false; // Reset raceStarted
                continue; // Go back to the start of the loop
            }

            if (IRSensorRead(1) == 1) {
                Serial.println("validation IR sensor detected");
                isValid = false;

                for (int blink = 0; blink < 5; blink++) {
                    Serial.print(blink);
                    Serial.println(". LED blinking");
                    lightLEDs(0xFF, 0x00, 0x00, NUM_LEDS, 1); // Light up all LEDs in red color
                    _delay_ms(500); // Delay
                    lightLEDs(0x00, 0x00, 0x00, NUM_LEDS, 1); // Turn off all LEDs
                    _delay_ms(500); // Delay
                }

                Serial.println("delay start");
                delayMs(200);
                Serial.println("delay finished");

                raceStarted = false; // Reset raceStarted

                Serial.println("validation finished");
            }

            if (isValid) {
                initLED(); // Reinitialize the LED strip
                startTime = microsSinceStart();
                Serial.print("startTime: ");
                Serial.println(startTime);

                Serial.println("measuring distance...");

                if (!mode) { // Track mode
                    while(!raceFinished) {
                        
                        uint16_t dist = measureDistanceCm(1);
                        
                        INDEX_LEDS = dist * 2.5;
                        lightLEDs(0xFF, 0x00, 0xFF, 6, INDEX_LEDS);
                        // for (int i=0; i < 144; i++) {
                        //     if (i > INDEX_LEDS - 6 && i < INDEX_LEDS) {
                        //         sendColor(0xFF, 0x00, 0xFF); // Green
                        //     } else {
                        //         sendColor(0x00, 0x00, 0x00); // Green
                        //     }
                        // }
                        delayMs(1);
                        counter++;
                        Serial.println(counter);
                    }
                } else { // Ghost mode
                    while (!raceFinished) {
                         
                    }
                }
            }
            isValid = true;
        } else {
            standby(mode);
        }
    }

    return 0;
}

// For start race button
ISR(PCINT0_vect) {
    switch (buttonState) {
        case WAIT_PRESS:
            delayMs(3);
            buttonState = DB_PRESS;
            break;

        case DB_PRESS:
            delayMs(3);
            cli();
            
            if ((PINB & (1 << PB3))) {
            Serial.println("PB3 pressed");
            Serial.println("start button triggered");
            
            StartCountdown(); // ✅ Trigger countdown and “GO !!!”
            
            raceStarted = true;

            initLED(); // Reinitialize the LED strip
            
            Serial.println("ISR finished");
            } else if ((PINB & (1 << PB5))) {
                // PB5 pressed (active-low)
                Serial.println("PB5 pressed");
                mode ^= 1;
                Serial.println("red button pressed");
                Serial.print("mode: ");
                Serial.println(mode ? "ghost" : "track");
            }


            buttonState = WAIT_PRESS;
            sei();
            break;

        case WAIT_RELEASE:
            delayMs(3);
            buttonState = DB_RELEASE;
            break;

        case DB_RELEASE:
            delayMs(3);
            buttonState = WAIT_PRESS;
            break;

        default:
            break;
    }
}

// For selecting mode button
// ISR(PCINT1_vect) {
//     // Serial.println("red button pressed");

//     // delayMs(30);
//     // // cli();
//     // mode ^= 1;
//     // Serial.println("red button pressed");
//     // Serial.print("mode: ");
//     // Serial.println(mode ? "ghost" : "race");
//     // buttonState = WAIT_PRESS;
//     // sei();
//     switch (buttonState) {
//         case WAIT_PRESS:
//             delayMs(30);
//             buttonState = DB_PRESS;
//             break;

//         case DB_PRESS:
//             delayMs(30);
//             cli();
//             mode ^= 1;
//             Serial.println("red button pressed");
//             Serial.print("mode: ");
//             Serial.println(mode ? "ghost" : "race");
//             buttonState = WAIT_PRESS;
//             sei();
//             break;
//     }
// }


// For IR at finish line
ISR(PCINT2_vect) {
    raceFinished = true;
}
