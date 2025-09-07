#ifndef LCD_H
#define LCD_H

#include <avr/io.h>
#include <util/delay.h>
#include <compat/twi.h>

// LCD I2C address (usually 0x27 or 0x3F depending on your module)
#define LCD_I2C_ADDRESS 0x27

// LCD Control bits
#define LCD_BACKLIGHT 0x08  // On
#define LCD_ENABLE    0x04  // Enable bit
#define LCD_RW        0x02  // Read/Write bit
#define LCD_RS        0x01  // Register select bit

// I2C initialization
void i2c_init(void) {
    TWSR = 0x00; // Set prescaler bits to 0
    TWBR = 0x0C; // SCL frequency is 400kHz with 16MHz clock
    TWCR = (1 << TWEN); // Enable TWI (I2C)
}

// Send START condition on I2C
void i2c_start(void) {
    TWCR = (1 << TWSTA) | (1 << TWEN) | (1 << TWINT); // Send START condition
    while (!(TWCR & (1 << TWINT))); // Wait for START to be transmitted
}

// Send STOP condition on I2C
void i2c_stop(void) {
    TWCR = (1 << TWSTO) | (1 << TWEN) | (1 << TWINT); // Send STOP condition
}

// Write data to I2C
void i2c_write(uint8_t data) {
    TWDR = data; // Load data to data register
    TWCR = (1 << TWEN) | (1 << TWINT); // Start transmission of data
    while (!(TWCR & (1 << TWINT))); // Wait for data to be transmitted
}

// Enable data transmission to LCD
void lcd_enable(uint8_t data) {
    i2c_start();
    i2c_write(LCD_I2C_ADDRESS << 1); // Send address with write bit
    i2c_write(data | LCD_ENABLE);    // Send data with enable bit set
    _delay_us(1);
    i2c_write(data & ~LCD_ENABLE);   // Clear enable bit
    _delay_us(50);
    i2c_stop();
}

// Send data/command to the LCD
void lcd_send(uint8_t data, uint8_t mode) {
    uint8_t highNibble = (data & 0xF0) | mode | LCD_BACKLIGHT;
    uint8_t lowNibble = ((data << 4) & 0xF0) | mode | LCD_BACKLIGHT;
    
    lcd_enable(highNibble);
    lcd_enable(lowNibble);
}

// Send command to the LCD
void lcd_command(uint8_t cmd) {
    lcd_send(cmd, 0);
}
	
// Print string on the LCD
void lcd_print(char *str) {
    while (*str) {
        lcd_send(*str, LCD_RS);
        str++;
    }
}

// Clear the LCD screen
void lcd_clear(void) {
    lcd_command(0x01); // Clear display command
    _delay_ms(2);      // Wait for the command to execute
}

// Set cursor position on the LCD
void lcd_setCursor(uint8_t col, uint8_t row) {
    uint8_t row_offsets[] = {0x00, 0x40, 0x14, 0x54};
    lcd_command(0x80 | (col + row_offsets[row]));  // Set DDRAM address
}

// Initialize the LCD
void initialize(void) {
    _delay_ms(50);        // Wait for LCD to power up
    lcd_command(0x02);    // Initialize in 4-bit mode
    lcd_command(0x28);    // 2 line, 5x7 matrix
    lcd_command(0x0C);    // Display on, cursor off
    lcd_command(0x06);    // Increment cursor
    lcd_clear();          // Clear display
}

#endif // LCD_H

