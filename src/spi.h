#ifndef SPI_H
#define SPI_H

#include <avr/interrupt.h>
#include <avr/io.h>

void SPI_MASTER_Init();

void SPI_Transmit(unsigned char data);

void MAX7219_Send(uint8_t address, uint8_t data);

void MAX7219_Init();

#endif