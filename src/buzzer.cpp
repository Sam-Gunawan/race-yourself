#include <avr/io.h>
#include "timer.h"


// #define F_CPU 16000000UL

// --- NOTES (partial list) ---
#define NOTE_B0 31
#define NOTE_C1 33
#define NOTE_CS1 35
#define NOTE_D1 37
#define NOTE_DS1 39
#define NOTE_E1 41
#define NOTE_F1 44
#define NOTE_FS1 46
#define NOTE_G1 49
#define NOTE_GS1 52
#define NOTE_A1 55
#define NOTE_AS1 58
#define NOTE_B1 62
#define NOTE_C2 65
#define NOTE_CS2 69
#define NOTE_D2 73
#define NOTE_DS2 78
#define NOTE_E2 82
#define NOTE_F2 87
#define NOTE_FS2 93
#define NOTE_G2 98
#define NOTE_GS2 104
#define NOTE_A2 110
#define NOTE_AS2 117
#define NOTE_B2 123
#define NOTE_C3 131
#define NOTE_CS3 139
#define NOTE_D3 147
#define NOTE_DS3 156
#define NOTE_E3 165
#define NOTE_F3 175
#define NOTE_FS3 185
#define NOTE_G3 196
#define NOTE_GS3 208
#define NOTE_A3 220
#define NOTE_AS3 233
#define NOTE_B3 247
#define NOTE_C4 262
#define NOTE_CS4 277
#define NOTE_D4 294
#define NOTE_DS4 311
#define NOTE_E4 330
#define NOTE_F4 349
#define NOTE_FS4 370
#define NOTE_G4 392
#define NOTE_GS4 415
#define NOTE_A4 440
#define NOTE_AS4 466
#define NOTE_B4 494
#define NOTE_C5 523
#define NOTE_CS5 554
#define NOTE_D5 587
#define NOTE_DS5 622
#define NOTE_E5 659
#define NOTE_F5 698
#define NOTE_FS5 740
#define NOTE_G5 784
#define NOTE_GS5 831
#define NOTE_A5 880
#define NOTE_AS5 932
#define NOTE_B5 988
#define NOTE_C6 1047
#define NOTE_CS6 1109
#define NOTE_D6 1175
#define NOTE_DS6 1245
#define NOTE_E6 1319
#define NOTE_F6 1397
#define NOTE_FS6 1480
#define NOTE_G6 1568
#define NOTE_GS6 1661
#define NOTE_A6 1760
#define NOTE_AS6 1865
#define NOTE_B6 1976
#define NOTE_C7 2093
#define NOTE_CS7 2217
#define NOTE_D7 2349
#define NOTE_DS7 2489
#define NOTE_E7 2637
#define NOTE_F7 2794
#define NOTE_FS7 2960
#define NOTE_G7 3136
#define NOTE_GS7 3322
#define NOTE_A7 3520
#define NOTE_AS7 3729
#define NOTE_B7 3951
#define NOTE_C8 4186
#define NOTE_CS8 4435
#define NOTE_D8 4699
#define NOTE_DS8 4978

// --- SONG DATA ---

struct Note {
    uint16_t frequency;
    uint16_t duration; // duration in ms
};

// Mario Theme Melody
Note marioTheme[] = {
    {NOTE_E7, 125}, {NOTE_E7, 125}, {0, 125}, {NOTE_E7, 125},
    {0, 125}, {NOTE_C7, 125}, {NOTE_E7, 125}, {0, 125},
    {NOTE_G7, 125}, {0, 375}, {NOTE_G6, 125}, {0, 375},

    {NOTE_C7, 125}, {0, 250}, {NOTE_G6, 125}, {0, 250},
    {NOTE_E6, 125}, {0, 250},

    {NOTE_A6, 125}, {0, 125}, {NOTE_B6, 125}, {0, 125}, {NOTE_AS6, 125}, {NOTE_A6, 125},

    {NOTE_G6, 125}, {NOTE_E7, 125}, {NOTE_G7, 125}, {NOTE_A7, 125},
    {NOTE_F7, 125}, {NOTE_G7, 125}, {0, 125},

    {NOTE_E7, 125}, {NOTE_C7, 125}, {NOTE_D7, 125}, {NOTE_B6, 125},
    {0, 375},

    {NOTE_C7, 125}, {0, 250}, {NOTE_G6, 125}, {0, 250},
    {NOTE_E6, 125}, {0, 250},

    {NOTE_A6, 125}, {0, 125}, {NOTE_B6, 125}, {0, 125}, {NOTE_AS6, 125}, {NOTE_A6, 125},

    {NOTE_G6, 125}, {NOTE_E7, 125}, {NOTE_G7, 125}, {NOTE_A7, 125},
    {NOTE_F7, 125}, {NOTE_G7, 125}, {0, 125},

    {NOTE_E7, 125}, {NOTE_C7, 125}, {NOTE_D7, 125}, {NOTE_B6, 125},
    {0, 375}
};

const uint8_t numNotes = sizeof(marioTheme) / sizeof(Note);

// Setup Timer4 PWM on PH4 (Pin 7)
void setupPWM()
{
    DDRH |= (1 << PH4);

    TCCR4A = (1 << COM4B1) | (1 << WGM41);
    TCCR4B = (1 << WGM43) | (1 << WGM42) | (1 << CS41); // Prescaler 8
}

// Play a note at given frequency
void playNote(uint16_t frequency)
{
    if (frequency == 0)
    {
        OCR4B = 0; // Silent
        return;
    }

    uint16_t top = (F_CPU / 8 / frequency) - 1;
    ICR4 = top;
    OCR4B = top / 2; // 50% duty cycle
}

// Delay ms
// void delayMs(uint16_t ms)
// {
//     while (ms--)
//         delayMs(1);
// }

// void playCountdown() {
//     // "3" - Mid tone (D5), longer
//     playNote(NOTE_D5);
//     delayMs(100);
//     playNote(0);
//     delayMs(100);

//     // "2" - Slightly higher (E5), same duration
//     playNote(NOTE_E5);
//     delayMs(100);
//     playNote(0);
//     delayMs(100);

//     // "1" - Higher (G5), same duration
//     playNote(NOTE_G5);
//     delayMs(100);
//     playNote(0);
//     delayMs(150);

//     // "GO!" - Quick happy burst, use chord-like feel
//     playNote(NOTE_C6);
//     delayMs(150);
//     playNote(0);
//     delayMs(100);
// }



void buzzer_main()
{

    //playCountdown();

    while (1)
    {
        for (uint8_t i = 0; i < numNotes; ++i)
        {
            playNote(marioTheme[i].frequency);
            delayMs(marioTheme[i].duration);
        }

        delayMs(500); // Little pause before restarting
    }
}