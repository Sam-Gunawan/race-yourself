#ifndef MATRIX_H
#define MATRIX_H

#include <Arduino.h>
#include "spi.h"

// Function prototypes for controlling the matrix
void displayFace(uint8_t face[8]);          // Display a single 8x8 pattern
void displayCountDown();                    // Display the countdown (3, 2, 1, etc.)
void StartCountdown();                      // Start the countdown
void displayGoMessage();                    // Display "GO !!!" as a scrolling text

// You may also want to add any constants or macros related to the matrix
#define MAX_MATRIX_COLS 8 // Number of columns on the matrix
#define MAX_MATRIX_ROWS 8 // Number of rows on the matrix

#endif // MATRIX_H
