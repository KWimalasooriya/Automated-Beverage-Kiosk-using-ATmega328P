#include <avr/io.h>
#include <util/delay.h>
#include "i2c.h"          // Include the I2C LCD header file

// Function prototypes
void setup();
uint8_t isButton1Pressed();  // Check if Button 1 is pressed
uint8_t isButton2Pressed();  // Check if Button 2 is pressed
void displayProcessing();    // Display "Processing" and "Auto Mode"
void displayChoosePercentages();  // Display "Choose the percentages"
void displayFruit(char *fruit, uint8_t percentage);  // Display fruit name with percentage
void displayOrderComplete();  // Display final order message
uint8_t mapPotentiometer(uint16_t value);  // Map potentiometer value to percentage

int main(void) {
    uint8_t button1State = 0;
    uint8_t lastButton1State = 0;
    uint8_t button2State = 0;
    uint8_t lastButton2State = 0;
    uint8_t fruitIndex = 0;  // To track which fruit to display
    char *fruits[] = {"PINEAPPLE", "MANGO", "APPLE", "ORANGE"};

    setup();                // Initialize pins
    LCD_Init();             // Initialize LCD

    // Initial display of "Auto Mode" and "Manual Mode"
    LCD_Clear();
    LCD_String("1. Auto Mode");    // Write initial string on the 1st row
    LCD_Command(0x80 | 0x40);      // Move cursor to 2nd row
    LCD_String("2. Manual Mode");  // Write string on the 2nd row
    
    while (1) {
        // Check if Button 1 (PC3) is pressed
        button1State = isButton1Pressed();

        // Falling edge detection for Button 1 (first button)
        if (button1State && !lastButton1State) {
            // Show "Processing" and "Auto Mode"
            displayProcessing();
            _delay_ms(300);  // Wait for 3 seconds

            // Show "Choose the percentages"
            displayChoosePercentages();
            _delay_ms(300);  // Wait for 3 seconds

            // Display the first fruit after the 3-second intervals
            uint16_t potValue = ADC;  // Read the potentiometer value (ADC value 0-1023)
            uint8_t percentage = mapPotentiometer(potValue);  // Map it to percentage (0% to 100%)
            displayFruit(fruits[fruitIndex], percentage);
            fruitIndex++;
        }
        lastButton1State = button1State;

        // Check if Button 2 (PC2) is pressed
        button2State = isButton2Pressed();

        // Falling edge detection for Button 2 (second button)
        if (button2State && !lastButton2State) {
            if (fruitIndex < 4) {
                // Show next fruit
                uint16_t potValue = ADC;  // Read the potentiometer value (ADC value 0-1023)
                uint8_t percentage = mapPotentiometer(potValue);  // Map it to percentage (0% to 100%)
                displayFruit(fruits[fruitIndex], percentage);
                fruitIndex++;
            } else {
                // If all fruits are displayed, show the final order message
                displayOrderComplete();  // Display "Your order is" and "on the way"
                fruitIndex = 0;  // Reset the fruit index
            }
        }
        lastButton2State = button2State;

        // Small delay for debouncing
        _delay_ms(100);
    }

    return 0;
}

// Function to set up the button and potentiometer pins
void setup() {
    // Set PC3 (Button 1) and PC2 (Button 2) as input
    DDRC &= ~(1 << PC3);
    DDRC &= ~(1 << PC2);

    // Enable pull-up resistors on PC3 and PC2
    PORTC |= (1 << PC3);
    PORTC |= (1 << PC2);

    // Set up the ADC (Analog-to-Digital Converter) for the potentiometer
    ADMUX = (1 << REFS0);            // Reference voltage is AVcc
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1);  // Enable ADC and set prescaler to 64
}

// Function to check if Button 1 is pressed
uint8_t isButton1Pressed() {
    return !(PINC & (1 << PC3));  // Return true (1) if Button 1 is pressed
}

// Function to check if Button 2 is pressed
uint8_t isButton2Pressed() {
    return !(PINC & (1 << PC2));  // Return true (1) if Button 2 is pressed
}

// Function to display "Processing" and "Auto Mode"
void displayProcessing() {
    LCD_Clear();                 // Clear the LCD
    LCD_String("Processing");     // Display "Processing" on the 1st row
    LCD_Command(0x80 | 0x40);     // Move cursor to 2nd row
    LCD_String("Auto Mode...");      // Display "Auto Mode" on the 2nd row
}

// Function to display "Choose the percentages"
void displayChoosePercentages() {
    LCD_Clear();                 // Clear the LCD
    LCD_String("Choose the");     // Display "Choose the" on the 1st row
    LCD_Command(0x80 | 0x40);     // Move cursor to 2nd row
    LCD_String("Percentages");    // Display "percentages" on the 2nd row
}

// Function to display fruit name and percentage
void displayFruit(char *fruit, uint8_t percentage) {
    char buffer[16];
    LCD_Clear();                 // Clear the LCD
    snprintf(buffer, 16, "%s %d%%", fruit, percentage);  // Format string: "Fruit Name XX%"
    LCD_String(buffer);          // Display the fruit and percentage
}

// Function to display "Your order is" and "on the way"
void displayOrderComplete() {
    LCD_Clear();                 // Clear the LCD
    LCD_String("Your order is");  // Display "Your order is" on the 1st row
    LCD_Command(0x80 | 0x40);     // Move cursor to 2nd row
    LCD_String("on the way");     // Display "on the way" on the 2nd row
}

// Function to map potentiometer value (0-1023) to percentage (0-100%)
uint8_t mapPotentiometer(uint16_t value) {
    return (value / 204) * 20;  // Map 0-1023 to 0%, 20%, 40%, ..., 100%
}

