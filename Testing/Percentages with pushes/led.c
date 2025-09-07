#define F_CPU 1000000
#include <avr/io.h>
#include <util/delay.h>
#include "i2c.h"  // Include the I2C LCD header file

// Function prototypes
void setup();
uint8_t isSwitch1Pressed();
uint8_t isEncoderPressed();
uint8_t isSwitch2Pressed();
uint8_t isSwitch3Pressed();
void displayAutoManual();
void displayProcessing();
void displayChoosePercentages();
void displayFruit(char *fruit, uint8_t percentage);
void displayExceededMessage();
void displayOrderComplete();

// Variables
char *fruits[] = {"PINEAPPLE", "MANGO", "APPLE", "ORANGE"};
uint8_t fruitIndex = 0;
uint8_t percentages[4] = {0, 0, 0, 0};  // Array to store percentages for each fruit
uint8_t percentage = 0;
uint8_t selectingPercentage = 0;

int main(void) {
    uint8_t switch1State = 0;
    uint8_t lastSwitch1State = 0;
    uint8_t encoderSwitchState = 0;
    uint8_t lastEncoderSwitchState = 0;

    setup();  // Initialize pins
    LCD_Init();  // Initialize LCD

    displayAutoManual();  // Initial display

    while (1) {
        // Check if Switch 1 (PC0) is pressed
        switch1State = isSwitch1Pressed();

        if (switch1State && !lastSwitch1State) {
            // Start the auto mode sequence
            displayProcessing();
            _delay_ms(4000);

            displayChoosePercentages();
            _delay_ms(4000);

            LCD_Clear();
            LCD_String("Total should not");
            LCD_Command(0xC0);  // Move to 2nd row
            LCD_String("exceed 100%");
            _delay_ms(4000);

            // Start selecting percentage for the first fruit
            fruitIndex = 0;
            percentage = 0;
            selectingPercentage = 1;  // Enable percentage selection
            displayFruit(fruits[fruitIndex], percentage);
        }
        lastSwitch1State = switch1State;

        // Check encoder only during the percentage selection phase
        if (selectingPercentage) {
            // Check if Switch 2 (PC1) is pressed to increase percentage
            if (isSwitch2Pressed() && percentage < 100) {
                percentage += 20;
                if (percentage > 100) {
                    percentage = 100; // Cap at 100%
                }
                displayFruit(fruits[fruitIndex], percentage);  // Update the displayed percentage
                _delay_ms(200);  // Debounce delay
            }

            // Check if Switch 3 (PC2) is pressed to decrease percentage
            if (isSwitch3Pressed() && percentage > 0) {
                percentage -= 20;
                if (percentage < 0) {
                    percentage = 0; // Floor at 0%
                }
                displayFruit(fruits[fruitIndex], percentage);  // Update the displayed percentage
                _delay_ms(200);  // Debounce delay
            }

            // Check if the encoder button is pressed to confirm the percentage
            encoderSwitchState = isEncoderPressed();
            if (encoderSwitchState && !lastEncoderSwitchState) {
                percentages[fruitIndex] = percentage;  // Store the selected percentage
                fruitIndex++;  // Move to the next fruit

                if (fruitIndex < 4) {
                    percentage = 0;  // Reset percentage for the next fruit
                    displayFruit(fruits[fruitIndex], percentage);  // Display next fruit
                } else {
                    selectingPercentage = 0;  // Disable encoder

                    // Check if the total exceeds 100%
                    uint8_t totalPercentage = percentages[0] + percentages[1] + percentages[2] + percentages[3];
                    if (totalPercentage > 100) {
                        displayExceededMessage();
                        _delay_ms(3000);

                        // Restart the selection process
                        fruitIndex = 0;
                        percentage = 0;
                        selectingPercentage = 1;
                        displayFruit(fruits[fruitIndex], percentage);
                    } else {
                        displayOrderComplete();
                        while (1);  // Stop the program
                    }
                }
            }
            lastEncoderSwitchState = encoderSwitchState;
        }

        // Small delay for debouncing
        _delay_ms(50);
    }

    return 0;
}

// Function to set up the button and encoder pins
void setup() {
    DDRC &= ~(1 << PC0);  // Set PC0 (Switch 1) as input
    PORTC |= (1 << PC0);  // Enable pull-up resistor on PC0

    DDRC &= ~(1 << PC1);  // Set PC1 (Switch 2) as input
    PORTC |= (1 << PC1);  // Enable pull-up resistor on PC1

    DDRC &= ~(1 << PC2);  // Set PC2 (Switch 3) as input
    PORTC |= (1 << PC2);  // Enable pull-up resistor on PC2

    // Encoder setup (only button is used now)
    DDRB &= ~(1 << PB1);  // CLK as input
    DDRB &= ~(1 << PB2);  // DT as input
    DDRB &= ~(1 << PB3);  // SW (encoder button) as input
    PORTB |= (1 << PB3);  // Enable pull-up resistor for encoder button
}

// Function to check if Switch 1 is pressed
uint8_t isSwitch1Pressed() {
    return !(PINC & (1 << PC0));
}

// Function to check if Switch 2 is pressed
uint8_t isSwitch2Pressed() {
    return !(PINC & (1 << PC1));
}

// Function to check if Switch 3 is pressed
uint8_t isSwitch3Pressed() {
    return !(PINC & (1 << PC2));
}

// Function to check if rotary encoder switch is pressed
uint8_t isEncoderPressed() {
    return !(PINB & (1 << PB3));
}

// Function to display "1. Auto Mode" and "2. Manual Mode"
void displayAutoManual() {
    LCD_Clear();
    LCD_String("1. Auto Mode");
    LCD_Command(0xC0);  // Move to the 2nd row
    LCD_String("2. Manual Mode");
}

// Function to display "Processing" and "Auto Mode"
void displayProcessing() {
    LCD_Clear();
    LCD_String("Processing");
    LCD_Command(0xC0);  // Move to the 2nd row
    LCD_String("Auto Mode...");
}

// Function to display "Select the percentages"
void displayChoosePercentages() {
    LCD_Clear();
    LCD_String("Select the");
    LCD_Command(0xC0);  // Move to the 2nd row
    LCD_String("Percentages...");
}

// Function to display a fruit and its percentage
void displayFruit(char *fruit, uint8_t percentage) {
    char buffer[16];
    LCD_Clear();
    LCD_String(fruit);
    LCD_Command(0xC0);  // Move to the 2nd row
    snprintf(buffer, 16, "%d%%", percentage);
    LCD_String(buffer);
}

// Function to display "Exceeded 100%" and "Select Again"
void displayExceededMessage() {
    LCD_Clear();
    LCD_String("Exceeded 100%");
    LCD_Command(0xC0);  // Move to the 2nd row
    LCD_String("Select Again");
}

// Function to display "Your order is" and "on the way"
void displayOrderComplete() {
    LCD_Clear();
    LCD_String("Your order is");
    LCD_Command(0xC0);  // Move to the 2nd row
    LCD_String("on the way");
}

