#include <avr/io.h>

#define IN1 PC0  // Connected to IN1 of L293D for motor 1
#define IN2 PC1  // Connected to IN2 of L293D for motor 1
#define EN1 PC2  // Connected to EN1 (Enable) of L293D for motor 1

#define IN3 PC3  // Connected to IN3 of L293D for motor 2
#define IN4 PC4  // Connected to IN4 of L293D for motor 2
#define EN2 PC5  // Connected to EN2 (Enable) of L293D for motor 2

void delay_ms(uint16_t ms) {
    // Simple delay function
    for(uint16_t i = 0; i < ms; i++)
        for(uint16_t j = 0; j < 800; j++);
}

// Motor 1 control functions
void motor1_forward() {
    PORTC &= ~(1 << IN1);  // Set IN1 low
    PORTC |= (1 << IN2);   // Set IN2 high
}

void motor1_stop() {
    PORTC &= ~((1 << IN1) | (1 << IN2)); // Set both IN1 and IN2 low (Motor OFF)
}

void motor1_enable() {
    PORTC |= (1 << EN1);  // Enable motor 1 by setting EN1 high
}

void motor1_disable() {
    PORTC &= ~(1 << EN1); // Disable motor 1 by setting EN1 low
}

// Motor 2 control functions
void motor2_forward() {
    PORTC &= ~(1 << IN3);  // Set IN3 low
    PORTC |= (1 << IN4);   // Set IN4 high
}

void motor2_stop() {
    PORTC &= ~((1 << IN3) | (1 << IN4)); // Set both IN3 and IN4 low (Motor OFF)
}

void motor2_enable() {
    PORTC |= (1 << EN2);  // Enable motor 2 by setting EN2 high
}

void motor2_disable() {
    PORTC &= ~(1 << EN2); // Disable motor 2 by setting EN2 low
}

int main(void) {
    // Set PC0 to PC5 as output for both motors
    DDRC |= (1 << IN1) | (1 << IN2) | (1 << EN1) | (1 << IN3) | (1 << IN4) | (1 << EN2);

    // Initially stop both motors and enable them
    motor1_stop();
    motor2_stop();
    motor1_enable();
    motor2_enable();

    while(1) {
        // Move motor 1 forward for 2 seconds
        motor1_forward();
        delay_ms(2000);

        // Stop motor 1 for 1 second
        motor1_stop();
        delay_ms(1000);

        // Move motor 2 forward for 2 seconds
        motor2_forward();
        delay_ms(2000);

        // Stop motor 2 for 1 second
        motor2_stop();
        delay_ms(1000);
    }

    return 0;
}

