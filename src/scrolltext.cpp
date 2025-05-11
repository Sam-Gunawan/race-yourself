#include <avr/io.h>
#include <util/delay.h>
#include "spi.h"



// ==================== Message Settings ====================
#define MAX_MSG_LEN     64
#define MAX_MSG_LENG 32
#define MAX_SCROLL_BUF  512

char message[MAX_MSG_LEN + 1] = {0};
uint8_t scrollBuffer[MAX_SCROLL_BUF];
uint16_t scrollLen = 0;

// ==================== Character Set (A–Z + space) ====================
const uint8_t alphabet[27][8] = {
    {0x18,0x24,0x42,0x7E,0x42,0x42,0x42,0x00}, // A
    {0x7C,0x42,0x42,0x7C,0x42,0x42,0x7C,0x00}, // B
    {0x3C,0x42,0x40,0x40,0x40,0x42,0x3C,0x00}, // C
    {0x78,0x44,0x42,0x42,0x42,0x44,0x78,0x00}, // D
    {0x7E,0x40,0x40,0x7C,0x40,0x40,0x7E,0x00}, // E
    {0x7E,0x40,0x40,0x7C,0x40,0x40,0x40,0x00}, // F
    {0x3C,0x42,0x40,0x4E,0x42,0x42,0x3C,0x00}, // G
    {0x42,0x42,0x42,0x7E,0x42,0x42,0x42,0x00}, // H
    {0x7E,0x18,0x18,0x18,0x18,0x18,0x7E,0x00}, // I
    {0x1E,0x04,0x04,0x04,0x44,0x44,0x38,0x00}, // J
    {0x42,0x44,0x48,0x70,0x48,0x44,0x42,0x00}, // K
    {0x40,0x40,0x40,0x40,0x40,0x40,0x7E,0x00}, // L
    {0x42,0x66,0x5A,0x5A,0x42,0x42,0x42,0x00}, // M
    {0x42,0x62,0x52,0x4A,0x46,0x42,0x42,0x00}, // N
    {0x3C,0x42,0x42,0x42,0x42,0x42,0x3C,0x00}, // O
    {0x7C,0x42,0x42,0x7C,0x40,0x40,0x40,0x00}, // P
    {0x3C,0x42,0x42,0x42,0x4A,0x44,0x3A,0x00}, // Q
    {0x7C,0x42,0x42,0x7C,0x48,0x44,0x42,0x00}, // R
    {0x3C,0x42,0x40,0x3C,0x02,0x42,0x3C,0x00}, // S
    {0x7E,0x18,0x18,0x18,0x18,0x18,0x18,0x00}, // T
    {0x42,0x42,0x42,0x42,0x42,0x42,0x3C,0x00}, // U
    {0x42,0x42,0x42,0x42,0x42,0x24,0x18,0x00}, // V
    {0x42,0x42,0x42,0x5A,0x5A,0x66,0x42,0x00}, // W
    {0x42,0x24,0x18,0x18,0x18,0x24,0x42,0x00}, // X
    {0x42,0x24,0x18,0x18,0x18,0x18,0x18,0x00}, // Y
    {0x7E,0x04,0x08,0x10,0x20,0x40,0x7E,0x00}, // Z
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}  // SPACE
};


// ==================== Mirror ====================
uint8_t reverseBits(uint8_t b) {
    b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
    b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
    b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
    return b;
}


// ==================== Utility Functions ====================
char toUpper(char c) {
    if (c >= 'a' && c <= 'z') return c - ('a' - 'A');
    return c;
}

const uint8_t* getCharBitmap(char c) {
    c = toUpper(c);
    if (c >= 'A' && c <= 'Z') return alphabet[c - 'A'];
    return alphabet[26];  // space or unknown
}

// // ==================== Hardware Control ====================
// void initPins() {
//     DATA_IN_DDR |= (1 << DATA_IN_PIN);
//     CLK_DDR     |= (1 << CLK_PIN);
//     CS_DDR      |= (1 << CS_PIN);
//     CS_PORT     |= (1 << CS_PIN);  // Idle high
// }

// void shiftOut(uint8_t data) {
//     for (int8_t i = 7; i >= 0; i--) {
//         if (data & (1 << i)) DATA_IN_PORT |= (1 << DATA_IN_PIN);
//         else                 DATA_IN_PORT &= ~(1 << DATA_IN_PIN);

//         CLK_PORT |= (1 << CLK_PIN);
//         _delay_us(1);
//         CLK_PORT &= ~(1 << CLK_PIN);
//         _delay_us(1);
//     }
// }

// // DIPAKE
// void sendByte(uint8_t reg, uint8_t data) {
//     CS_PORT &= ~(1 << CS_PIN);
//     shiftOut(reg);
//     shiftOut(data);
//     CS_PORT |= (1 << CS_PIN);
// }

// void clearMatrix() {
//     for (uint8_t i = 1; i <= 8; i++) {
//         sendByte(i, 0x00);
//     }
// }

// void setupMAX7219() {
//     sendByte(0x09, 0x00); // No decode
//     sendByte(0x0A, 0x03); // Brightness
//     sendByte(0x0B, 0x07); // Scan all digits
//     sendByte(0x0C, 0x01); // Normal mode
//     sendByte(0x0F, 0x00); // Display test off
//     clearMatrix();
// }

// ==================== Message Functions ====================
void setMessage(const char* newMsg) {
    int i = 0;
    while (newMsg[i] != '\0' && i < MAX_MSG_LENG) {
        message[i] = newMsg[i];
        i++;
    }
    message[i] = '\0';  // Null-terminate the string
}


int strLength(const char* str) {
    int len = 0;
    while (str[len] != '\0') {
        len++;
    }
    return len;
}

// void buildScrollBuffer() {
//     scrollLen = 0;
//     int messageLen = strLength(message);  // Replaces strlen(message)

//     for (uint8_t i = 0; i < messageLen; i++) {
//         const uint8_t* bitmap = getCharBitmap(message[i]);
//         // Build columns from rows (transpose)
//         for (int8_t col = 7; col >= 0; col--) {  // REVERSED COLUMN ORDER
//             uint8_t transposed = 0;
//             for (uint8_t row = 0; row < 8; row++) {
//                 if (bitmap[row] & (1 << col)) {
//                     transposed |= (1 << (7 - row));
//                 }
//             }
//             scrollBuffer[scrollLen++] = transposed;
//         }
//         scrollBuffer[scrollLen++] = 0x00; // space between characters
//     }
// }

void buildScrollBuffer() {
    scrollLen = 0;
    int messageLen = strLength(message);

    for (uint8_t i = 0; i < messageLen; i++) {
        const uint8_t* bitmap = getCharBitmap(message[i]);
        for (uint8_t col = 0; col < 8; col++) {
            scrollBuffer[scrollLen++] = bitmap[col];
        }
        scrollBuffer[scrollLen++] = 0x00; // space between characters
    }
}



void scrollMessage() {
    buildScrollBuffer();
    for (uint16_t i = 0; i < scrollLen - 8; i++) {
        for (uint8_t col = 0; col < 8; col++) {
            // Send rightmost buffer column to digit 1, leftmost to digit 8
            MAX7219_Send(8 - col, scrollBuffer[i + col]);
        }
        _delay_ms(30); // Adjust scroll speed
    }
}




// ==================== Main ====================


// DIPAKE
// int main(void) {
//     initPins();
//     setupMAX7219();

//     setMessage("My Name is Calvin");  // 🔧 Change your message here

//     while (1) {
//         scrollMessage();
//     }
// }