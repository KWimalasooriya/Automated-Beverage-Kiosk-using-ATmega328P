#define F_CPU 1000000
#include <avr/io.h>
#include <util/delay.h>
#include "LCD.h"

// Function prototypes
void setup();
uint8_t isSwitch1Pressed();
uint8_t isEncoderPressed();
void displayWelcome();
void displayProcessing();
void displayChoosePercentages();
void displayExceed100();
void displayFruit(char *fruit, uint8_t percentage);
void displayOrderComplete();
int8_t readEncoder();
void checkPercentageSum();

// Variables
char *fruits[] = {"PINEAPPLE", "MANGO", "APPLE", "ORANGE"};
uint8_t fruitIndex = 0;
uint8_t percentages[4] = {0, 0, 0, 0};  // Array to store percentages for each fruit
uint8_t percentage = 0;
uint8_t selectingPercentage = 0;
uint8_t switch1Pressed = 0;

int main(void) {
    setup();  // Initialize pins
    initialize();  // Initialize LCD

    displayWelcome();  // Display "Welcome" at the start

    // Wait for Switch 1 (PC1) press
    while (!isSwitch1Pressed());

    // Disable Switch 1 after the first press
    switch1Pressed = 1;

    // Display "Processing.." and other startup messages
    displayProcessing();
    _delay_ms(4000);

    displayChoosePercentages();
    _delay_ms(4000);

    lcd_clear();
    lcd_setCursor(0, 0);
    lcd_print("Total should not");
    lcd_setCursor(0, 1);
    lcd_print("exceed 100%");
    _delay_ms(4000);

    // Begin the fruit and percentage selection process
    displayFruit(fruits[fruitIndex], percentage);
    selectingPercentage = 1;  // Enable percentage selection

    while (1) {
        if (selectingPercentage) {
            // Read the rotary encoder to adjust the percentage
            int8_t rotation = readEncoder();
            if (rotation > 0 && percentage < 100) {
                percentage += 20;
                displayFruit(fruits[fruitIndex], percentage);  // Update the displayed percentage
            } else if (rotation < 0 && percentage > 0) {
                percentage -= 20;
                displayFruit(fruits[fruitIndex], percentage);  // Update the displayed percentage
            }

            // Check if the rotary encoder switch is pressed to confirm the percentage and move to the next fruit
            if (isEncoderPressed()) {
                _delay_ms(50); // Debounce delay
                if (isEncoderPressed()) { // Confirm switch press after delay
                    percentages[fruitIndex] = percentage;  // Store the selected percentage
                    fruitIndex++;  // Move to the next fruit

                    if (fruitIndex < 4) {
                        percentage = 0;  // Reset percentage for the next fruit
                        displayFruit(fruits[fruitIndex], percentage);  // Display next fruit
                    } else {
                        selectingPercentage = 0;  // Disable encoder
                        checkPercentageSum();  // Check if total exceeds 100
                    }
                }
            }
        }
        _delay_ms(50);  // Small delay for debouncing
    }

    return 0;
}

// Function to set up the button and encoder pins
void setup() {
    DDRC &= ~(1 << PC0);  // Set PC0 (Switch 1) as input
    PORTC |= (1 << PC0);  // Enable pull-up resistor on PC0

    DDRB &= ~(1 << PB1);  // Set PB1 (CLK) as input
    DDRB &= ~(1 << PB2);  // Set PB2 (DT) as input
    DDRB &= ~(1 << PB3);  // Set PB3 (SW) as input
    PORTB |= (1 << PB1) | (1 << PB2) | (1 << PB3);  // Enable pull-up resistors for encoder
}

// Function to check if Switch 1 (PC1) is pressed
uint8_t isSwitch1Pressed() {
    return !(PINC & (1 << PC0));
}

// Function to check if rotary encoder switch is pressed
uint8_t isEncoderPressed() {
    return !(PINB & (1 << PB3));
}

// Function to display "Welcome" at the beginning
void displayWelcome() {
    lcd_clear();
    lcd_setCursor(4, 0);  // Display "Welcome" in the middle
    lcd_print("Welcome");
}

// Function to display "Processing.." message for 4 seconds
void displayProcessing() {
    lcd_clear();
    lcd_setCursor(0, 0);
    lcd_print("Processing..");
}

// Function to display "Select the" and "Percentages.." message
void displayChoosePercentages() {
    lcd_clear();
    lcd_setCursor(0, 0);
    lcd_print("Select the");
    lcd_setCursor(0, 1);
    lcd_print("Percentages..");
}

// Function to display exceeded 100% message
void displayExceed100() {
    lcd_clear();
    lcd_setCursor(0, 0);
    lcd_print("Exceeded 100%");
    lcd_setCursor(0, 1);
    lcd_print("Select again");
    _delay_ms(4000);
}

// Function to display a fruit and its percentage
void displayFruit(char *fruit, uint8_t percentage) {
    char buffer[16];
    lcd_clear();
    lcd_setCursor(0, 0);
    lcd_print(fruit);
    lcd_setCursor(0, 1);
    snprintf(buffer, 16, "%d%%", percentage);
    lcd_print(buffer);
}

// Function to display "Your order is" and "on the way"
void displayOrderComplete() {
    lcd_clear();
    lcd_setCursor(0, 0);
    lcd_print("Your order");
    lcd_setCursor(0, 1);
    lcd_print("is on the way");
}

// Function to check the total percentage
void checkPercentageSum() {
    uint8_t total = percentages[0] + percentages[1] + percentages[2] + percentages[3];
    if (total > 100) {
        displayExceed100();  // If total exceeds 100%, allow re-selection
        fruitIndex = 0;  // Reset to the first fruit
        percentage = 0;
        displayFruit(fruits[fruitIndex], percentage);
        selectingPercentage = 1;  // Re-enable encoder for selection
    } else {
        displayOrderComplete();  // If total is valid, display order message
    }
}

// Function to read rotary encoder rotation
int8_t readEncoder() {
    static uint8_t lastStateCLK = 0;
    uint8_t currentStateCLK = PINB & (1 << PB1);

    if (currentStateCLK != lastStateCLK) {
        if (PINB & (1 << PB2)) {
            lastStateCLK = currentStateCLK;
            return 1;  // Clockwise rotation
        } else {
            lastStateCLK = currentStateCLK;
            return -1;  // Counterclockwise rotation
        }
    }
    return 0;  // No rotation
}
