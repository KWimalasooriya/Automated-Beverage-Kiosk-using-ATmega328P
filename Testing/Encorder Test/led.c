#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "i2c.h"  // Include I2C LCD header file

#define F_CPU 1000000UL   // Set CPU frequency to 1 MHz

#define encClk PB1      // CLK pin of the rotary encoder (PB1)
#define encDT PB3       // DT pin of the rotary encoder (PB3)
#define encButton PB2   // Button pin of the rotary encoder (PB2)
#define switch1 PC0     // Switch 1 (PC0)
#define SDA PC4         // I2C SDA pin for the LCD
#define SCL PC5         // I2C SCL pin for the LCD

volatile int position = 0;  // Tracks the position of the rotary encoder (0 to 5, representing 0% to 100%)
volatile int percentage = 0;  // Holds the percentage value

// Function prototypes
void setup();
void rotaryEncISR();
void buttonISR();
void updateLCDValue(int value);

int main(void) {
    setup();
    
    LCD_Init();  // Initialize I2C LCD
    LCD_Clear();  // Clear the LCD
    LCD_String("Value: ");  // Display "Value: " initially on the first line

    sei();  // Enable global interrupts

    while (1) {
        // Main loop can be left empty as everything is handled by the ISRs
    }

    return 0;
}

void setup() {
    // Set rotary encoder pins as inputs with pull-up resistors
    DDRB &= ~(1 << encClk) & ~(1 << encDT) & ~(1 << encButton);  // Set PB1, PB3, PB2 as inputs
    PORTB |= (1 << encClk) | (1 << encDT) | (1 << encButton);    // Enable pull-up resistors

    // Set switch1 as input with pull-up resistor
    DDRC &= ~(1 << switch1);  // Set PC0 as input
    PORTC |= (1 << switch1);  // Enable pull-up resistor

    // External interrupts for rotary encoder
    EICRA |= (1 << ISC00);  // Trigger on any change for PB1 (INT0)
    EIMSK |= (1 << INT0);   // Enable INT0 interrupt

    // Enable pin change interrupt for the button (PB2)
    PCICR |= (1 << PCIE0);  // Enable pin change interrupt on PCIE0
    PCMSK0 |= (1 << PB2);   // Enable interrupt on PB2
}

// ISR for rotary encoder (triggered on any change in PB1)
ISR(INT0_vect) {
    rotaryEncISR();
}

// ISR for button press (triggered on pin change in PB2)
ISR(PCINT0_vect) {
    if (!(PINB & (1 << encButton))) {  // Button pressed (active low)
        buttonISR();
    }
}

void rotaryEncISR() {
    // Update rotary encoder position (6 positions: 0%, 20%, 40%, 60%, 80%, 100%)
    if (PINB & (1 << encClk)) {
        if (!(PINB & (1 << encDT))) {
            position++;  // Clockwise rotation
        } else {
            position--;  // Counter-clockwise rotation
        }
    } else {
        if (!(PINB & (1 << encDT))) {
            position--;  // Counter-clockwise rotation
        } else {
            position++;  // Clockwise rotation
        }
    }

    // Limit position within the range [0, 5]
    if (position > 5) {
        position = 5;
    } else if (position < 0) {
        position = 0;
    }

    // Map position to percentage based on clock positions
    switch (position) {
        case 0:
            percentage = 0;
            break;
        case 1:
            percentage = 20;
            break;
        case 2:
            percentage = 40;
            break;
        case 3:
            percentage = 60;
            break;
        case 4:
            percentage = 80;
            break;
        case 5:
            percentage = 100;
            break;
    }

    updateLCDValue(percentage);  // Update LCD with the new percentage
}

void buttonISR() {
    // Reset the rotary encoder to the 0% position on button press
    position = 0;
    percentage = 0;
    updateLCDValue(percentage);  // Reset percentage on the LCD
}

void updateLCDValue(int value) {
    char buffer[5];
    snprintf(buffer, 5, "%d%%", value);  // Format the value as percentage
    
    LCD_SetCursor(1, 0);  // Move the cursor to the second line of the LCD
    LCD_String("            ");  // Clear the second line (16 characters)
    
    LCD_SetCursor(1, 0);  // Set cursor to the beginning of the second line again
    LCD_String(buffer);   // Display the percentage value
}

