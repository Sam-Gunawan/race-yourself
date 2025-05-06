/* 
1. All communication with the 8 x 8 led matrix must be done over the SPI
related pins.
2. Read the 8x8 MAX7219 datasheet for setting up SPI mode.
*/

#include "spi.h"

#define DDR_SPI DDRB
#define DD_SS DDB0
#define DD_SCK DDB1
#define DD_MOSI DDB2
#define SPI_PORT PORTB
#define SPI_SS_BIT PORTB0

void SPI_MASTER_Init() {
    DDR_SPI = (1 << DD_MOSI) | (1 << DD_SCK) | (1 << DD_SS);

    SPI_PORT |= (1 << SPI_SS_BIT);

    SPCR |= (1 << SPE) | (1 << MSTR) | (1 << CPOL) | (1 << CPHA) | (1 << SPR1) | (1 << SPR0);
}

void SPI_Transmit(unsigned char data) {
    SPDR = data; // Load data into the buffer
    while (!(SPSR & (1 << SPIF))); // Wait until transmission is complete
}

void MAX7219_Send(uint8_t address, uint8_t data) {
    // Pull SS low to select the device
    SPI_PORT &= ~(1 << SPI_SS_BIT);
    
    SPI_Transmit(address); // Send register address
    SPI_Transmit(data);    // Send the data

    // Pull SS high to finish transmission
    SPI_PORT |= (1 << SPI_SS_BIT);
}

void MAX7219_Init() {
    // Intensity: medium
    MAX7219_Send(0x0A, 0x08);

    // Scan limit: all 8 digits
    MAX7219_Send(0x0B, 0x07);

    // Shutdown register: normal operation
    MAX7219_Send(0x0C, 0x01);

    // Display test: off
    MAX7219_Send(0x0F, 0x00);
}