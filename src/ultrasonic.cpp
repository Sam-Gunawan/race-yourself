#include "ultrasonic.h"
#include "timer.h"

// ==== Pin Definitions ====
#define TRIG_PIN  PE4  // Digital Pin 2
#define ECHO_PIN  PE5  // Digital Pin 3

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
    DDRE |= (1 << TRIG_PIN);    // TRIG as output
    DDRE &= ~(1 << ECHO_PIN);   // ECHO as input
}

uint16_t measureDistanceCm() {
    uint32_t count = 0;

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

// ==== Main ====
int main(void) {
    char buf[32];

    UART_init(103);         // 16MHz, 9600 baud
    initUltrasonic();

    while (1) {
        uint16_t dist = measureDistanceCm();

        snprintf(buf, sizeof(buf), "Distance: %u cm\r\n", dist);
        UART_SendString(buf);

        delayMs(200); // 5 times per second
    }
}