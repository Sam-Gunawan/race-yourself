#include <FastLED.h>

// Define the number of LEDs and the data pin
#define NUM_LEDS 144
#define DATA_PIN 45

// Create an array to hold the LED data
CRGB leds[NUM_LEDS];

void setup() {
    // Initialize the LED strip
    FastLED.addLeds<WS2812, DATA_PIN, GRB>(leds, NUM_LEDS);
    FastLED.clear(); // Clear all LEDs
    FastLED.show();  // Ensure LEDs are off initially
}

void loop() {
    // Create a white chase effect
    for (int i = 3; i < NUM_LEDS; i++) {
        leds[i-3] = CRGB::Black; // Turn off the previous LED
        leds[i-2] = CRGB(150, 150, 150);
        leds[i-1] = CRGB(200, 200, 200);
        leds[i] = CRGB::White;                            // Bright white head
        FastLED.show(); // Update the LED strip
        delay(10);
    }
    delay(50);      // Small delay for smooth animation
}