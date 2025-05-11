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
#include "matrix.h"
#include "buzzer.h"


// TODO: TO BE IMPLEMENTED
#include "led.h" 


#define NUM_LEDS 144     // Number of LEDs on the strip

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

int INDEX_LEDS = 0;
int counter = 0;



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
void defaultDisplay() {
    // Clears the display
    clearDisplay();

    // Idle display (waiting for track to start)
    moveCursor(0, 0);                      // First line
    writeString("Race Yourself");
    moveCursor(0, 1);                      // Second line
    writeString("Idle...");
}

void standby() {
    defaultDisplay();
    for (int i = 0; i < 144; i++) {
        sendColor(0xCC, 0xCC, 0xCC); // White
    }
    delayMs(1);
}


int main() {
    initEverything();

    //char buf[32];

    while (1) {
        initLED(); // Reinitialize the LED strip

        if (raceStarted) {
            initLED(); // Reinitialize the LED strip

            if (raceFinished) {
                initLED(); // Reinitialize the LED strip
                finishTime = microsSinceStart();
                Serial.println("Finish time: ");
                Serial.println(finishTime);

                raceDuration = finishTime - startTime;

                Serial.print("Race time: ");
                Serial.println(counter/1000); // Convert to seconds

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
                    lightLEDs(4, NUM_LEDS, 1); // Light up all LEDs in red color
                    delayMs(50); // Delay
                    lightLEDs(0, NUM_LEDS, 1); // Turn off all LEDs
                    delayMs(50); // Delay
                }

                Serial.println("delay start");
                delayMs(200);
                Serial.println("delay finished");

                Serial.println("validation buzzer triggered");
                // buzzer_main();
                Serial.println("validation finished");

                raceStarted = false; // Reset raceStarted
            }

            if (isValid) {
                initLED(); // Reinitialize the LED strip
                startTime = microsSinceStart();
                Serial.print("startTime: ");
                Serial.println(startTime);

                Serial.println("measuring distance...");
                while(!raceFinished) {
                    // char buf[32];
                    //UART_SendString(buf);
            
                    uint16_t dist = measureDistanceCm(1);
                    // snprintf(buf, sizeof(buf), "Distance: %u cm\r\n", dist);
                    INDEX_LEDS = dist * 2.5;

                    for (int i=0; i < 144; i++) {
                        if (i > INDEX_LEDS - 6 && i < INDEX_LEDS) {
                            sendColor(0xFF, 0x00, 0xFF); // Green
                        } else {
                            sendColor(0x00, 0x00, 0x00); // Green
                        }
                    }
                    delayMs(1);
                    // counter++;
                }
            }
        } else {
            lightLEDs(6, NUM_LEDS, 1); // Light up all LEDs in yellow color
        }
    }

    return 0;
}

// void ir_detect(){
//     int tim = 0;
//     for (int i = 0; i < 10; i++)
//     {
//         if (IRSensorRead(1) == 1){
//             Serial.println("bad, not detected");
//             break;
//         }
//         if (i >=1499){
//             tim = i;
//         }
//         delayMs(2);
//     }
//     if (tim >= 1499) {
//         Serial.println("Good");
//     }
// }

ISR(PCINT0_vect) {
    switch (buttonState) {
        case WAIT_PRESS:
            delayMs(3);
            buttonState = DB_PRESS;
            break;

        case DB_PRESS:
            delayMs(3);
            cli();
            Serial.println("start button triggered");
            
            StartCountdown(); // ✅ Trigger countdown and “GO !!!”
            // for (int i = 0; i < NUM_LEDS; i++) {
            //     sendColor(0x01, 0x00, 0x00);
            // }

            raceStarted = true;

            initLED(); // Reinitialize the LED strip
            
            Serial.println("ISR finished");

            buttonState = WAIT_RELEASE;
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

ISR(PCINT2_vect) {
    // while (!(PINK & (1 << PK2)));
    raceFinished = true;
}
