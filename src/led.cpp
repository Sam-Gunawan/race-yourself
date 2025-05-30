#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>

#include "led.h"
#include "timer.h"

// Pin connected to DIN of WS2812B
#define LED_PORT PORTB
#define LED_DDR  DDRB
#define LED_PIN  PB0
#define NUM_LEDS 144

void initLED() {
    LED_DDR |= (1 << LED_PIN); // Set pin as output
    LED_PORT &= ~(1 << LED_PIN); // Set pin low
}

void nopDelay(uint8_t n) {
    while(n--) {
        asm volatile ("nop\n\t");
    }
}

void sendBit(uint8_t bitVal) {
    if (bitVal) {
        // Send '1' bit
        LED_PORT |= (1 << LED_PIN);       // High
        // asm volatile ("nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t");
        nopDelay(12);
        LED_PORT &= ~(1 << LED_PIN);      // Low
        // asm volatile ("nop\n\t""nop\n\t""nop\n\t");
        nopDelay(3);
    } else {
        // Send '0' bit
        LED_PORT |= (1 << LED_PIN);       // High
        // asm volatile ("nop\n\t""nop\n\t""nop\n\t");
        nopDelay(3);
        LED_PORT &= ~(1 << LED_PIN);      // Low
        // asm volatile (
        //     "nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t"
        //     "nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t"
        // );
        nopDelay(11);
    }
}

void sendByte(uint8_t byte) {
    for (uint8_t i = 0; i < 8; i++) {
        sendBit(byte & 0xFF);
        byte <<= 1;
    }
}

void sendColor(uint8_t r, uint8_t g, uint8_t b) {
    cli(); // Disable interrupts
    sendByte(g); // WS2812B expects GRB order
    sendByte(r);
    sendByte(b);
    sei(); // Re-enable interrupts
}

/*
 * lightLEDs - Function to light up the LEDs in a specific color and width
 * @param color - Color to light the LEDs:
        color=0 : RGB(0, 0, 0) - Black (off)
        color=1 : RGB(0, 0, 1) - Blue
        color=2 : RGB(0, 1, 0) - Green
        color=3 : RGB(0, 1, 1) - Cyan
        color=4 : RGB(1, 0, 0) - Red
        color=5 : RGB(1, 0, 1) - Magenta
        color=6 : RGB(1, 1, 0) - Yellow
        color=7 : RGB(1, 1, 1) - White
 * @param width - Number of LEDs to light up (default: NUM_LEDS, light everything up)
 * @param start - Starting index of the LEDs to light up (default: 1)
 */
void lightLEDs(int color=0, int width=NUM_LEDS, int start=1) {
    int LEDs[NUM_LEDS][3] = { {0, 0, 0} }; // Initialize all LEDs to off (black)

    int startIndex = start - 1;
    int endIndex = startIndex + width;

    // Clamp to valid LED range
    if (startIndex < 0) startIndex = 0;
    if (endIndex > NUM_LEDS) endIndex = NUM_LEDS;

    uint8_t r = 0, g = 0, b = 0;

    switch(color) {
        case 0: r = 0x00; g = 0x00; b = 0x00; break; // Off
        case 1: r = 0x00; g = 0x00; b = 0x10; break; // Blue
        case 2: r = 0x00; g = 0x10; b = 0x00; break; // Green
        case 3: r = 0x00; g = 0x10; b = 0x10; break; // Cyan
        case 4: r = 0x10; g = 0x00; b = 0x00; break; // Red
        case 5: r = 0x10; g = 0x00; b = 0x10; break; // Magenta
        case 6: r = 0x10; g = 0x10; b = 0x00; break; // Yellow
        case 7: r = 0x10; g = 0x10; b = 0x10; break; // White
        default: r = 0x00; g = 0x00; b = 0x00; break;
    }

    for (int i = startIndex; i < endIndex; i++) {
        LEDs[i][0] = g;
        LEDs[i][1] = r;
        LEDs[i][2] = b;
    }

    // Send data to LEDs
    for (int i = 0; i < NUM_LEDS; i++) {
        sendColor(LEDs[i][0], LEDs[i][1], LEDs[i][2]);
    }

    delayMs(1); // Small delay to allow latch
}
