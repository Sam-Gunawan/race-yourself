#include "ultrasonic.h"
#include "timer.h"

// ==== Pin Definitions ====
#define TRIG_PIN1  PE4  // Digital Pin 2
#define ECHO_PIN1  PE5  // Digital Pin 3
#define TRIG_PIN2  PF0  // Digital Pin 2
#define ECHO_PIN2  PF1  // Digital Pin 3


// ==== UART Setup ====
void UART_init(uint16_t ubrr) {
    UBRR0H = (uint8_t)(ubrr >> 8);
    UBRR0L = (uint8_t)ubrr;
    UCSR0B = (1 << TXEN0);                    // Enable TX
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);   // 8-bit
}

void UART_SendChar(char c) {
    while (!(UCSR0A & (1 << UDRE0)));
    UDR0 = c;
}

void UART_SendString(const char* s) {
    while (*s) UART_SendChar(*s++);
}

// ==== HC-SR04 Setup ====
void initUltrasonic() {
    DDRE |= (1 << TRIG_PIN1);    // TRIG as output for the 1st sensor
    DDRE &= ~(1 << ECHO_PIN1);   // ECHO as input for the 1st sensor
    DDRE |= (1 << TRIG_PIN2);    // TRIG as output for the 2nd sensor
    DDRE &= ~(1 << ECHO_PIN2);   // ECHO as input for the 2nd sensor
}

uint16_t measureDistanceCm(int number) {
    uint32_t count = 0;
    uint8_t TRIG_PIN;
    uint8_t ECHO_PIN;

    if (number == 1){
        TRIG_PIN = TRIG_PIN1;
        ECHO_PIN = ECHO_PIN1;
    } else if (number == 2){
        TRIG_PIN = TRIG_PIN2;
        ECHO_PIN = ECHO_PIN2;
    }
    
    // Trigger 10us pulse
    PORTE &= ~(1 << TRIG_PIN);
    delayUs(2);
    PORTE |= (1 << TRIG_PIN);
    delayUs(10);
    PORTE &= ~(1 << TRIG_PIN);

    // Wait for ECHO HIGH (timeout)
    uint32_t timeout = 60000;
    while (!(PINE & (1 << ECHO_PIN)) && timeout--) delayUs(1);
    if (timeout == 0) return 0;

    // Measure ECHO HIGH pulse duration
    count = 0;
    while ((PINE & (1 << ECHO_PIN))) {
        delayUs(1);
        count++;
        if (count > 30000) break; // timeout ~500 cm
    }

    return count / 58;  // convert to cm
}