#ifndef SCROLLINGTEXT_H
#define SCROLLINGTEXT_H

#include <avr/interrupt.h>
#include <avr/io.h>



void initScrollingText();
void setMessage(const char* newMsg);
void scrollMessage();


#endif