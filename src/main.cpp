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
volatile bool raceStart = false;
volatile bool raceFinished = false;
volatile bool isValid = true;
volatile unsigned int startTime;
volatile unsigned int finishTime;
volatile unsigned int raceDuration;

int INDEX_LEDS = 0;



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
    for (int i=0; i < 144; i++) {
        sendColor(0x01, 0x00, 0x01);
    }
    delayMs(1000);
    for (int i=0; i < 144; i++) {
        sendColor(0x00, 0x00, 0x00);
    }
    delayMs(1000);
}


int main() {
    initEverything();
    
    //char buf[32];
    
    while (1) {
        

        if (!raceStart) {
            Serial.println(raceStart);
            standby();  // Only go to standby when race hasn't started
        } else {
            Serial.println("Race has started");

            for (int i = 0; i < 144; i++) {
                sendColor(0x00, 0x00, 0x00);  // Clear LEDs
            }
            delayMs(1);

            // Reset `raceStart` or handle race logic here

            // For now, after clearing, reset so standby resumes
            raceStart = false;
        }

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
        //     lightLEDs(3, 4, j); // Light up 4 LEDs in cyan color
        //     delayMs(1); // Delay for 100ms
        // }  


        /* IF THAT DOESN'T WORK, TRY THIS */
        // for (int j = 0; j < NUM_LEDS - 4; j++) {
        //     lightLEDs(3, 4, j); // Light up 4 LEDs in cyan color
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
            delayMs(30);
            buttonState = DB_PRESS;
            break;

        case DB_PRESS:
            delayMs(30);
            cli();
            Serial.println("start button triggered");
            
            StartCountdown(); // ✅ Trigger countdown and “GO !!!”
            for (int i = 0; i < NUM_LEDS; i++) {
                sendColor(0x01, 0x00, 0x00);
            }
            raceStart = true;

            if (IRSensorRead(1) == 1) {
                Serial.println("IR sensor read");
                Serial.println("LED Changing");
                delayMs(2000);
                buzzer_main();
                isValid = false;
            }

            if (isValid) {
                Serial.println("isValid entered");
                startTime = microsSinceStart();

                while(!raceFinished) {
                    Serial.println("Race loop entered");
                    char buf[32];
                    UART_SendString(buf);
            
                    uint16_t dist = measureDistanceCm(1);
                    snprintf(buf, sizeof(buf), "Distance: %u cm\r\n", dist);
                    INDEX_LEDS = dist * 2.5;

                    for (int i=0; i < 144; i++) {
                        if (i > INDEX_LEDS - 6 && i < INDEX_LEDS) {
                            sendColor(0x01, 0x01, 0x01); // Green
                        } else {
                            sendColor(0x00, 0x00, 0x00); // Green
                        }
                    }
                    delayMs(1);
                }
                
                raceDuration = finishTime - startTime;

                for (int blink = 0; blink < 3; blink++) {
                    for (int i = 0; i < NUM_LEDS; i++) {
                        sendColor(0x00, 0x01, 0x00);
                    }
                    delayMs(500);
                }


                Serial.print("Race time: ");
                Serial.println(raceDuration);
            }
            
            buttonState = WAIT_RELEASE;
            sei();
            break;

        case WAIT_RELEASE:
            delayMs(30);
            buttonState = DB_RELEASE;
            break;

        case DB_RELEASE:
            delayMs(30);
            buttonState = WAIT_PRESS;
            break;

        default:
            break;
    }
}

ISR(PCINT2_vect) {
    if (!(PINK & (1 << PK2))) {
        // Serial.println("3");
        finishTime = microsSinceStart();
        raceFinished = true;
    }
    // delayMs(10);
}
