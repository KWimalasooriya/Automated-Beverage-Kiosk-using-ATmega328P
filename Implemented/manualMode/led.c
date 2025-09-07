#define F_CPU 1000000
#include <avr/io.h>
#include <util/delay.h>
#include "LCD.h"

// Function prototypes
void setup();
uint8_t isSwitch1Pressed();
uint8_t isSwitch2Pressed();
uint8_t isEncoderPressed();
void displayModes();
void displayProcessing();
void displayChoosePercentages();
void displayExceed100();
void displayFruit(char *fruit, uint8_t percentage);
void displayOrderComplete();
void displayEnjoyDrink();
void turnOnMotor(uint8_t motor, uint8_t percentage);
void turnOffMotors();
int8_t readEncoder();
void checkPercentageSum();
uint16_t getDelayForPercentage(uint8_t percentage);
void manualMode();  // Function prototype for manual mode

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

    while (1) {
        displayModes();  // Display mode selection at the start

        // Wait for Switch 1 (PC0) press
        while (!isSwitch1Pressed()) {
            // Check if Switch 2 (PC1) is pressed for Manual Mode
            if (isSwitch2Pressed()) {
                // Disable Switches for Manual Mode
                switch1Pressed = 1;  // Disable Switch 1
                manualMode();        // Go to Manual Mode
            }
        }

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
                            break;  // Exit the inner loop
                        }
                    }
                }
            }
            _delay_ms(50);  // Small delay for debouncing
        }
    }

    return 0;
}

// Function to set up the button and encoder pins
void setup() {
    DDRC &= ~(1 << PC0);  // Set PC0 (Switch 1) as input
    PORTC |= (1 << PC0);  // Enable pull-up resistor on PC0

    DDRC &= ~(1 << PC1);  // Set PC1 (Switch 2) as input
    PORTC |= (1 << PC1);  // Enable pull-up resistor on PC1

    DDRB &= ~(1 << PB1);  // Set PB1 (CLK) as input
    DDRB &= ~(1 << PB2);  // Set PB2 (DT) as input
    DDRB &= ~(1 << PB3);  // Set PB3 (SW) as input
    PORTB |= (1 << PB1) | (1 << PB2) | (1 << PB3);  // Enable pull-up resistors for encoder

    // Set relay control pins as output (assuming PORTD)
    DDRD |= (1 << PD0) | (1 << PD1) | (1 << PD2) | (1 << PD3);  // Example pins for motors
    turnOffMotors();  // Ensure motors are off initially
}

// Function to check if Switch 1 (PC0) is pressed
uint8_t isSwitch1Pressed() {
    return !(PINC & (1 << PC0));
}

// Function to check if Switch 2 (PC1) is pressed
uint8_t isSwitch2Pressed() {
    return !(PINC & (1 << PC1));
}

// Function to check if rotary encoder switch is pressed
uint8_t isEncoderPressed() {
    return !(PINB & (1 << PB3));
}

// Function to display mode selection
void displayModes() {
    lcd_clear();
    lcd_setCursor(0, 0);  // Display "Welcome" in the middle
    lcd_print("1. Auto Mode");
    lcd_setCursor(0, 1);
    lcd_print("2. Manual Mode");
}

// Function to display "Processing.." message for 4 seconds
void displayProcessing() {
    lcd_clear();
    lcd_setCursor(0, 0);
    lcd_print("Processing");
    lcd_setCursor(0, 1);
    lcd_print("Auto Mode...");
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
    snprintf(buffer, sizeof(buffer), "%d%%", percentage);
    lcd_print(buffer);
}

// Function to display "Your order is" and "on the way"
void displayOrderComplete() {
    lcd_clear();
    lcd_setCursor(0, 0);
    lcd_print("Your order is");
    lcd_setCursor(0, 1);
    lcd_print("on the way");
}

// Function to display "Enjoy" and "Your drink"
void displayEnjoyDrink() {
    lcd_clear();
    lcd_setCursor(0, 0);
    lcd_print("Enjoy");
    lcd_setCursor(0, 1);
    lcd_print("Your drink");
    _delay_ms(4000);
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
        for (uint8_t i = 0; i < 4; i++) {
            turnOnMotor(i, percentages[i]);  // Turn on motors based on percentages
        }

        displayEnjoyDrink();  // Display enjoyment message
        fruitIndex = 0;  // Reset fruit index for new selection
        percentages[0] = percentages[1] = percentages[2] = percentages[3] = 0;  // Reset percentages
        displayModes();  // Return to mode selection
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

// Function to turn on the specified motor based on percentage
void turnOnMotor(uint8_t motor, uint8_t percentage) {
    uint16_t delayTime = getDelayForPercentage(percentage);
    PORTD &= ~(1 << motor);  // Turn on the motor

    // Use a loop for delay instead of _delay_ms directly
    while (delayTime > 0) {
        _delay_ms(1);  // Delay in 1 ms increments
        delayTime--;  // Decrease delayTime
    }

    PORTD |= (1 << motor);  // Turn off the motor after delay
}


// Function to turn off all motors
void turnOffMotors() {
    PORTD |= ((1 << PD0) | (1 << PD1) | (1 << PD2) | (1 << PD3));  // Turn off all motors
}

// Function to get delay for the corresponding percentage
uint16_t getDelayForPercentage(uint8_t percentage) {
    switch (percentage) {
        case 0: return 0;          // 0%
        case 20: return 2180;      // 2.18 seconds
        case 40: return 4110;      // 4.11 seconds
        case 60: return 5730;      // 5.73 seconds
        case 80: return 6970;      // 6.97 seconds
        case 100: return 8110;     // 8.11 seconds
        default: return 0;         // Invalid percentage
    }
}

// Function for Manual Mode
void manualMode() {
    lcd_clear();
    lcd_setCursor(0, 0);
    lcd_print("Processing");
    lcd_setCursor(0,1);
    lcd_print("Manual Mode...");
    _delay_ms(4000);

    lcd_clear();
    lcd_setCursor(0, 0);
    lcd_print("Select only");
    lcd_setCursor(0,1);
    lcd_print("One Fruit!");
    _delay_ms(4000);
    
    uint8_t selectedFruitIndex = 0; // Index for the currently selected fruit
    uint8_t fruitSelected = 0;       // Flag to check if a fruit is selected

    // Main loop for manual mode
    while (1) {
        // Display the currently selected fruit
        displayFruit(fruits[selectedFruitIndex], percentages[selectedFruitIndex]);
        
        // Read the rotary encoder to switch between fruits
        int8_t rotation = readEncoder();
        if (rotation > 0) {
            // Rotate clockwise to select the next fruit
            selectedFruitIndex++;
            if (selectedFruitIndex >= 4) {
                selectedFruitIndex = 0; // Wrap around
            }
            _delay_ms(200); // Debounce delay
        } else if (rotation < 0) {
            // Rotate counterclockwise to select the previous fruit
            if (selectedFruitIndex == 0) {
                selectedFruitIndex = 3; // Wrap around
            } else {
                selectedFruitIndex--;
            }
            _delay_ms(200); // Debounce delay
        }

        // Check if the rotary encoder switch is pressed to confirm selection
        if (isEncoderPressed()) {
            _delay_ms(50); // Debounce delay
            if (isEncoderPressed()) { // Confirm switch press after delay
                // Activate the selected fruit
                percentages[selectedFruitIndex] = 100; // Example: Set percentage to 100%
                turnOnMotor(selectedFruitIndex, percentages[selectedFruitIndex]); // Turn on the selected fruit motor
                fruitSelected = 1; // Set flag to indicate fruit is selected
                break; // Exit manual mode loop
            }
        }

        _delay_ms(50); // Small delay for debouncing
    }

    // After fruit selection
    if (fruitSelected) {
        displayOrderComplete();  // Display order message
        displayEnjoyDrink();      // Display enjoy message
    }
    
    // Return to mode selection or reset for another manual selection
    fruitIndex = 0;  // Reset fruit index for new selection
    percentages[0] = percentages[1] = percentages[2] = percentages[3] = 0;  // Reset percentages
    displayModes();  // Return to mode selection
}

