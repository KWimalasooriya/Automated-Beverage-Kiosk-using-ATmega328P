#define F_CPU 1000000
#include <avr/io.h>           /* Include AVR standard library file */
#include <util/delay.h>       /* Include Delay header file */
#include "LCD.h"              /* Include your I2C LCD header file */

int main(void) {
    DDRB |= (1 << PB0);      /* Set PB0 as output for LED indication */
    
    initialize();            /* Initialize the LCD */
    
    lcd_clear();             /* Clear the LCD display */
    lcd_setCursor(0, 0);     /* Set cursor to the first line, first position */
    lcd_print("1. Auto Mode"); /* Write string on 1st row */
    
    lcd_setCursor(0, 1);     /* Set cursor to the second line, first position */
    lcd_print("2. Manual Mode"); /* Write string on 2nd row */
    
    while (1) {
        PORTB ^= (1 << PB0);  /* Toggle LED on PB0 */
        _delay_ms(500);       /* Delay for LED blinking */
    }
    
    return 0;
}

