#include <avr/io.h>
#include <util/delay.h>    // Include for delay functions
#include "i2c.h"           // Include the I2C LCD header file

// Function prototypes
void setup();
uint8_t isButtonPressed();
void displayProcessing();  // Function to display "Processing" and "Auto Mode"

int main(void) {
    uint8_t buttonState = 0;       // Variable to store the current button state
    uint8_t lastButtonState = 0;   // Variable to track the previous state of the button (for edge detection)

    setup();                       // Initialize pins
    LCD_Init();                    // Initialize LCD

    // Initial LCD Display
    LCD_String("1. Auto Mode");    // Write initial string on the 1st row
    LCD_Command(0x80 | 0x40);      // Move cursor to 2nd row
    LCD_String("2. Manual Mode");  // Write string on the 2nd row

    while (1) {
        // Read the button state
        buttonState = isButtonPressed();

        // Detect if the button was just pressed (falling edge detection)
        if (buttonState && !lastButtonState) {
            // Update LCD to show "Processing" and "Auto Mode"
            displayProcessing();
        }

        // Save the current button state for the next loop
        lastButtonState = buttonState;

        // Small delay for debouncing
        _delay_ms(100);
    }

    return 0;
}

// Function to set up the button pins
void setup() {
    // Set PC3 as input for the button (NO pin)
    DDRC &= ~(1 << PC3);

    // Enable pull-up resistor on PC3
    PORTC |= (1 << PC3);
}

// Function to check if the button is pressed
uint8_t isButtonPressed() {
    return !(PINC & (1 << PC3));  // Return true (1) if button is pressed
}

// Function to update the LCD with "Processing" and "Auto Mode"
void displayProcessing() {
    LCD_Clear();                    // Clear the LCD
    LCD_String("Processing");        // Display "Processing" on the 1st row
    LCD_Command(0x80 | 0x40);        // Move cursor to 2nd row
    LCD_String("Auto Mode....");         // Display "Auto Mode" on the 2nd row
}

