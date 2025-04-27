#include <FastLED.h>

// Define the number of LEDs and the data pin
#define NUM_LEDS 144
#define DATA_PIN 16

// Create an array to hold the LED data
CRGB leds[NUM_LEDS];

void setup() {
    // Initialize the LED strip
    FastLED.addLeds<WS2812, DATA_PIN, GRB>(leds, NUM_LEDS);
    FastLED.clear(); // Clear all LEDs
    FastLED.show();  // Ensure LEDs are off initially
}

void loop() {
    static uint8_t hue = 0; // Starting hue value

    // Create a rainbow chase effect
    for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = CHSV((hue + i * 10) % 255, 255, 255); // Set color based on hue
    }
    FastLED.show(); // Update the LED strip
    hue++;          // Increment the hue for the next frame
    delay(50);      // Small delay for smooth animation
}



// Define pins for the ultrasonic sensor
#define TRIG_PIN 9
#define ECHO_PIN 10

void setupUltrasonicSensor() {
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);
}

long readUltrasonicDistance() {
    // Send a 10-microsecond pulse to trigger pin
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);

    // Read the echo pin and calculate distance
    long duration = pulseIn(ECHO_PIN, HIGH, 30000); // Timeout after 30ms
    if (duration == 0) {
        return -1; // Return -1 if no echo is received
    }
    long distance = duration * 0.034 / 2; // Convert to centimeters
    return distance;
}

void setup() {
    // Initialize the LED strip
    FastLED.addLeds<WS2812, DATA_PIN, GRB>(leds, NUM_LEDS);
    FastLED.clear(); // Clear all LEDs
    FastLED.show();  // Ensure LEDs are off initially

    // Initialize ultrasonic sensor and serial communication
    setupUltrasonicSensor();
    Serial.begin(9600);
}

void loop() {
    static uint8_t hue = 0; // Starting hue value

    // Create a rainbow chase effect
    for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = CHSV((hue + i * 10) % 255, 255, 255); // Set color based on hue
    }
    FastLED.show(); // Update the LED strip
    hue++;          // Increment the hue for the next frame
    long distance = readUltrasonicDistance(); // Read distance from the sensor
    if (distance == -1) {
        Serial.println("Distance: Out of range");
    } else {
        Serial.print("Distance: ");
        Serial.print(distance);
        Serial.println(" cm");
    }

    delay(50); // Small delay for smooth animation and sensor reading
}