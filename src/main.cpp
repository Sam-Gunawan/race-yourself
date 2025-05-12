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
#define DISTANCE 1      // The distance of the LED strip (in meters)

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
volatile bool mode = 0; // 0: track mode, 1: ghost mode
volatile bool ghostMode = false;
volatile double playerSpeed;

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

                raceTime = counter/9.17; // Convert to seconds
                playerSpeed = DISTANCE / raceTime; // Calculate the speed m/s
                
                Serial.print("Player time: ");
                Serial.println(raceTime);
                Serial.print("Player speed: ");
                Serial.println(playerSpeed);
                
                if (raceTime > ghostTime) {ghostTime = raceTime;} // Save fastest time record to ghost time

                Serial.print("Ghost time: ");
                Serial.println(ghostTime);

                // Check mode, different outputs for different outcomes
                // Ghost mode
                if (mode) {
                    // Player wins
                    if (raceTime > ghostTime) {
                        clearDisplay();
                        moveCursor(0, 0);
                        writeString("Ghost defeated!");

                        moveCursor(0, 1);
                        char speedString[16];
                        sprintf(speedString, "Speed: %.2f", playerSpeed);  // Keep 2 decimal places
                        writeString(speedString);

                        // Green LED breathing animation
                        for (int breathe = 0; breathe < 10; breathe++) {
                            for (int brightness = 0; brightness <= 255; brightness += 5) {
                                lightLEDs(0x00, brightness, 0x00, NUM_LEDS, 1); // Gradually increase brightness
                                _delay_ms(10); // Delay for smooth animation
                            }
                            for (int brightness = 255; brightness >= 0; brightness -= 5) {
                                lightLEDs(0x00, brightness, 0x00, NUM_LEDS, 1); // Gradually decrease brightness
                                _delay_ms(10); // Delay for smooth animation
                            }
                        }

                    // Ghost wins
                    } else {
                        clearDisplay();
                        moveCursor(0, 0);
                        writeString("Try again!");

                        moveCursor(0, 1);
                        char speedString[16];
                        sprintf(speedString, "Speed: %.2f", playerSpeed);  // Keep 2 decimal places
                        writeString(speedString);

                        // Blue LED breathing animation
                        for (int breathe = 0; breathe < 10; breathe++) {
                            for (int brightness = 0; brightness <= 255; brightness += 5) {
                                lightLEDs(0x00, 0x00, brightness, NUM_LEDS, 1); // Gradually increase brightness
                                _delay_ms(10); // Delay for smooth animation
                            }
                            for (int brightness = 255; brightness >= 0; brightness -= 5) {
                                lightLEDs(0x00, 0x00, brightness, NUM_LEDS, 1); // Gradually decrease brightness
                                _delay_ms(10); // Delay for smooth animation
                            }
                        }
                    }

                // Track mode
                } else {
                    clearDisplay();
                    moveCursor(0, 0);
                    writeString("Race Finished!");
                    moveCursor(0, 1);

                    moveCursor(0, 1);
                    char speedString[16];
                    sprintf(speedString, "Speed: %.2f", playerSpeed);  // Keep 2 decimal places
                    writeString(speedString);

                    // Green LED breathing animation
                    for (int breathe = 0; breathe < 10; breathe++) {
                        for (int brightness = 0; brightness <= 255; brightness += 5) {
                            lightLEDs(0x00, brightness, 0x00, NUM_LEDS, 1); // Gradually increase brightness
                            _delay_ms(10); // Delay for smooth animation
                        }
                        for (int brightness = 255; brightness >= 0; brightness -= 5) {
                            lightLEDs(0x00, brightness, 0x00, NUM_LEDS, 1); // Gradually decrease brightness
                            _delay_ms(10); // Delay for smooth animation
                        }
                    }
                }

                // Reset all variables
                counter = 0;
                startTime = 0;
                raceFinished = false; // Reset raceFinished
                raceStarted = false; // Reset raceStarted
                continue; // Go back to the start of the loop
            }

            if (IRSensorRead(1) == 1) {
                Serial.println("validation IR sensor detected");
                isValid = false;
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
                    if (ghostTime == 0) {
                        isValid = false; // Set isValid to false if ghostTime is not set
                        continue; // Go back to the start of the loop
                    }

                    while (!raceFinished) {
                        // LED turns on with the average speed of the ghost
                        float ghostSpeed = DISTANCE / ghostTime; // Calculate the speed m/s
                        Serial.print("Ghost speed: ");
                        Serial.println(ghostSpeed);

                        // Convert to LED/s and find the delay time
                        // LED speed = ghostSpeed * LEDs/m = (m/s) * (LEDs/m) = LEDs/s
                        float ledSpeed = ghostSpeed * (NUM_LEDS/DISTANCE); // Convert to LED/s
                        Serial.print("LED speed: ");
                        Serial.println(ledSpeed);
                        float delayTime = 1000 / ledSpeed; // Delay time in ms
                        Serial.print("Delay time: ");
                        Serial.println(delayTime);

                        for (int startLED = 1; startLED < NUM_LEDS; startLED++) {
                            lightLEDs(0xFF, 0x00, 0xFF, 6, startLED);
                            delayMs(delayTime); // Delay time in ms
                        }
                    }
                }
            } else {
                Serial.println("Race invalid...");
                
                clearDisplay();
                moveCursor(0, 0);
                writeString("Race Invalid!");
                moveCursor(0, 1);
                if (!ghostTime) {writeString("No ghost saved");} // Player selected ghost mode when no record is saved previously
                else {writeString("No player");} // No player detected with IR at starting line

                for (int blink = 0; blink < 5; blink++) {
                    Serial.print(blink);
                    Serial.println(". Red LED blinking");
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

            isValid = true;
        } else {
            standby(mode);
        }
    }

    return 0;
}

// For start race and select mode button
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
