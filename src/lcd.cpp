#include "lcd.h"
#include "timer.h"

/*
 * Initializes all pins related to the LCD to be outputs
 */
void initLCDPins(){
  // Set all data pins PA0, PA1, PA2, PA3 as output
  DDRA |= (1 << DDA0) | (1 << DDA1) | (1 << DDA2) | (1 << DDA3);

  // Set RS and E pins as output
  DDRB |= (1 << DDB4) | (1 << DDB6);
}


/* 1. Assert bits to pins connected to DB[7:4] on the LCD screen
 *      a. data is an unsigned char which has 8 bits. Therefore, you
 *         need assign the bottom 4 bits of "data" to the appropriate bits in
 *         PORTA
 *  2. This is a "command" signal, meaning RS should be Low
 *  3. Assert high on enable pin, delay, and asset low on enable pin
 *  4. delay the provided number in MICROseconds.
 */
void fourBitCommandWithDelay(unsigned char data, unsigned int delay){
  // Get the first 4 bits of data
  PORTA = ((PORTA & 0xF0) | (data & 0xF0)) >> 4;

  // RS should be low for command
  PORTB &= ~(1 << PORTB6);

  // Enable pin high
  PORTB |= (1 << PORTB4);
  delayUs(1);

  // Enable pin low
  PORTB &= ~(1 << PORTB4);
  delayUs(delay);

}



/* Similar to fourBitCommandWithDelay except that now all eight bits of command are
 * sent.
 * 1. Assert bits to pins connected to DB[7:4] on the LCD screen
 *       a. unlike fourBitCommandWithDelay, you need to send the TOP four bits of
 *          "command" first. These should be assigned to appropriate bits in PORTA
 * 2. This is a command signal, meaning RS should be low
 * 3. Assert high on enable pin, delay, and asset low on enable pin
 * 4. Now set the lower four bits of command to appropriate bits in PORTA
 * 5. Assert high on enable pin, delay, and asset low on enable pin
 * 6. delay the provided number in MICROseconds.
 */
void eightBitCommandWithDelay(unsigned char command, unsigned int delay){
  
  // Get the first 4 bits of data
  PORTA = ((PORTA & 0xF0) | (command & 0xF0)) >> 4;

  // RS should be low for command
  PORTB &= ~(1 << PORTB6);
  
  // Enable pin high
  PORTB |= (1 << PORTB4);
  delayUs(1);

  // Enable pin low
  PORTB &= ~(1 << PORTB4);
  // delayUs(delay);

  // Get the last 4 bits of data
  PORTA = (PORTA & 0xF0) | (command & 0x0F);
  
  // RS should be low for command
  PORTB &= ~(1 << PORTB6);

  // Enable pin high
  PORTB |= (1 << PORTB4);
  delayUs(1);
  
  // Enable pin low
  PORTB &= ~(1 << PORTB4);
  delayUs(delay);
}



/* Similar to eightBitCommandWithDelay except that now RS should be high
 * 1. Assert bits to pins connected to DB[7:4] on the LCD screen
 * 2. This is a "data" signal, meaning RS should be high
 * 3. Assert high on enable pin, delay, and asset low on enable pin
 * 4. Now set the lower four bits of character to appropriate bits in PORTA
 * 5. Assert high on enable pin, delay, and asset low on enable pin
 * 6. delay is always 46 MICROseconds for a character write
 */
void writeCharacter(unsigned char character){
  int delay = 46;
  
  // RS should be high for data signal
  PORTB |= (1 << PORTB6);
    
  // Get the first 4 bits of data
  PORTA = ((PORTA & 0xF0) | (character & 0xF0)) >> 4;

  // Enable pin high
  PORTB |= (1 << PORTB4);
  delayUs(1);

  // Enable pin low
  PORTB &= ~(1 << PORTB4);
  // delayUs(delay);

  // Get the last 4 bits of data
  PORTA = (PORTA & 0xF0) | (character & 0x0F);

  // Enable pin high
  PORTB |= (1 << PORTB4);
  delayUs(1);

  // Enable pin low
  PORTB &= ~(1 << PORTB4);
  delayUs(delay); 
}




/*
 * Writes a provided string such as "Hello!" to the LCD screen. You should
 * remember that a c string always ends with the '\0' character and
 * that this should just call writeCharacter multiple times.
 */
void writeString(const char *string){
  while (*string != '\0') {
    writeCharacter(*string);
    string++;
  }
}



/*
 * This moves the LCD cursor to a specific place on the screen.
 * This can be done using the eightBitCommandWithDelay with correct arguments
 */
void moveCursor(unsigned char x, unsigned char y){
  unsigned char address;

  // Calculate the address based on x and y
  if (y == 0) {
    address = x;
  } else if (y == 1) {
    address = 0x40 + x;
  } else {
    // Invalid y value, do nothing
    return;
  }

  // Set the DDRAM address command (0x80 | address)
  eightBitCommandWithDelay(0x80 | address, 53);
}

void clearDisplay(){
  // Clear display command
  eightBitCommandWithDelay(0x01, 1000);
  // Move cursor to home position
  moveCursor(0, 0);
}

/* This is the procedure outline on the LCD datasheet page 4 out of 9.
 * This should be the last function you write as it largely depends on all other
 * functions working.
 */
void initLCDProcedure(){
  // Delay 15 milliseconds
  delayMs(15);

  // Write 0b0011 to DB[7:4] and delay 4100 microseconds
  fourBitCommandWithDelay(0x3, 4100);
  
  // Write 0b0011 to DB[7:4] and delay 100 microseconds
  fourBitCommandWithDelay(0x3, 100);

  // The data sheet does not make this clear, but at this point you are issuing
  // commands in two sets of four bits. You must delay after each command
  // (which is the second set of four bits) an amount specified on page 3 of
  // the data sheet.
  // write 0b0011 to DB[7:4] and 100us delay
  fourBitCommandWithDelay(0x3, 100);

  // write 0b0010 to DB[7:4] and 100us delay.
  fourBitCommandWithDelay(0x2, 100);
  // fourBitCommandWithDelay(0b0010, 100);

  // Function set in the command table with 53us delay
  eightBitCommandWithDelay(0x2, 53);
  eightBitCommandWithDelay(0x28, 53);
  // eightBitCommandWithDelay(0b00101011, 53);

  // Display off in the command table with 53us delay
  eightBitCommandWithDelay(0x08, 53);

  // Clear display in the command table. Remember the delay is longer!!!
  eightBitCommandWithDelay(0x01, 1000);

  // Entry Mode Set in the command table.
  eightBitCommandWithDelay(0x06, 53);

  // Display ON/OFF Control in the command table. (Yes, this is not specified),
  // in the data sheet, but you have to do it to get this to work. Yay datasheets!)
  eightBitCommandWithDelay(0x0C, 53);

}


/* Initializes Tri-state for LCD pins and calls initialization procedure.
* This function is made so that it's possible to test initLCDPins separately
* from initLCDProcedure which will likely be necessary.
*/
void initLCD(){
  initLCDPins();
  initLCDProcedure();
}