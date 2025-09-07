#define F_CPU 1000000
#include <avr/io.h>
#include <util/delay.h>

#define RELAY_PIN PD0  // Connected to IN pin of the Relay

void relay_on() {
    PORTD &= ~(1 << RELAY_PIN);  // Set the relay pin HIGH to turn ON the pump
}

void relay_off() {
    PORTD |= (1 << RELAY_PIN); // Set the relay pin LOW to turn OFF the pump
}

void delay_ms(uint16_t ms) {
    // Simple delay function using _delay_ms
    while(ms--) {
        _delay_ms(1);
    }
}

int main(void) {
    // Set RELAY_PIN as output
    DDRD |= (1 << RELAY_PIN);

    // Initially turn off the pump
    relay_off();

    while (1) {
        // Turn on the pump for 5 seconds
	delay_ms(3000);
        relay_on();
        delay_ms(10000);

        // Turn off the pump for 2 seconds
        relay_off();
        delay_ms(2000);
    }

    return 0;
}

