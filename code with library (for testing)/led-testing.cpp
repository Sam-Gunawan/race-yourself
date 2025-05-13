// #include <FastLED.h>

// // Define the number of LEDs and the data pin
// #define NUM_LEDS 14
// #define DATA_PIN 10

// // Create an array to hold the LED data
// CRGB leds[NUM_LEDS];

// // Define the purple color
// #define PURPLE CRGB(128, 0, 128) // Adjust RGB values for your preferred purple

// // Breathing effect parameters
// const int breathSpeed = 20; // Adjust for faster/slower breathing
// const int minBrightness = 20; // Minimum brightness (0-255)
// const int maxBrightness = 255; // Maximum brightness (0-255)
// int brightness = minBrightness;
// int fadeAmount = 5; // Adjust for smoother/steeper fade

// void setup() {
//   // Initialize the LED strip
//   FastLED.addLeds<WS2812, DATA_PIN, GRB>(leds, NUM_LEDS);
//   FastLED.clear(); // Clear all LEDs
//   FastLED.show();  // Ensure LEDs are off initially
// }

// void loop() {
//   // Set all LEDs to the current brightness of purple
//   for (int i = 0; i < NUM_LEDS; i++) {
//     leds[i] = PURPLE.scale8(brightness); // scale8 adjusts brightness
//   }
//   FastLED.show(); // Update the LED strip
//   delay(breathSpeed);

//   // Adjust brightness for the breathing effect
//   brightness = brightness + fadeAmount;
//   if (brightness <= minBrightness || brightness >= maxBrightness) {
//     fadeAmount = -fadeAmount; // Reverse the fading direction
//   }
// }

// // int main() {
// //   init(); // Initialize the Arduino hardware
// //   setup(); // Call your setup function once

// //   while (true) {
// //     loop();  // Call your loop function repeatedly
// //   }

// //   return 0; // This line is usually never reached in typical Arduino programs
// // }