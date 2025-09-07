#include <avr/io.h>
#include <avr/interrupt.h>
#include "i2c.h" // Make sure to include the LCD driver library (you should have this configured for AVR)

#define encClk PD2  // Rotary encoder CLK
#define encButton PD3 // Rotary encoder button
#define encQuad PD4  // Rotary encoder Quadrature pin

volatile int decCount = 0;
volatile int onesCount = 0;
volatile int tensCount = 0;

void setup() {
    // Set up pins for rotary encoder
    DDRD &= ~(1 << encClk);   // Set encClk as input
    DDRD &= ~(1 << encButton); // Set encButton as input
    DDRD &= ~(1 << encQuad);  // Set encQuad as input
    PORTD |= (1 << encClk) | (1 << encButton) | (1 << encQuad); // Enable internal pull-up resistors

    // Set up interrupts
    EICRA |= (1 << ISC00);    // Set INT0 to trigger on rising edge
    EIMSK |= (1 << INT0);     // Enable external interrupt on INT0
    sei();                    // Enable global interrupts

    // Initialize LCD
    lcd_init();               // Initialize the LCD
    lcd_clear();
    lcd_gotoxy(0, 0);
    lcd_puts("Value: ");      // Display static label

    lcd_gotoxy(0, 1);
    lcd_puts("000");          // Initialize value display with "000"
}

void loop() {
    // Loop does nothing, everything is handled by interrupts
}

ISR(INT0_vect) {
    // Rotary encoder ISR for count up/down
    if (PIND & (1 << encClk)) {  // If CLK is HIGH
        if (PIND & (1 << encQuad)) {
            countUp();
        } else {
            countDown();
        }
    } else {
        if (PIND & (1 << encQuad)) {
            countDown();
        } else {
            countUp();
        }
    }
    updateDisplay(); // Update the display whenever the encoder is turned
}

void countUp(void) {
    decCount++;
    if (decCount > 9) {
        decCount = 0;
        onesCount++;
    }
    if (onesCount > 9) {
        onesCount = 0;
        tensCount++;
    }
    if (tensCount > 9) {
        tensCount = 0;
    }
}

void countDown(void) {
    decCount--;
    if (decCount < 0) {
        decCount = 9;
        onesCount--;
    }
    if (onesCount < 0) {
        onesCount = 9;
        tensCount--;
    }
    if (tensCount < 0) {
        tensCount = 9;
    }
}

void updateDisplay(void) {
    char value[4];
    snprintf(value, sizeof(value), "%d%d%d", tensCount, onesCount, decCount);  // Convert counts to string

    lcd_gotoxy(0, 1);          // Move to the second row
    lcd_puts(value);           // Display the updated value
}

