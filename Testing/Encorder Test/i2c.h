#ifndef I2C_H
#define I2C_H

#include <avr/io.h>           /* Include AVR std. library file */
#include <util/delay.h>       /* Include Delay header file */

/* LCD I2C address */
#define LCD_I2C_ADDRESS 0x27  /* Define I2C address of the LCD, often 0x27 or 0x3F */
#define LCD_BACKLIGHT 0x08    /* Backlight control bit */
#define ENABLE 0x04           /* Enable bit */
#define READ_WRITE 0x02       /* Read/Write bit */
#define REGISTER_SELECT 0x01  /* Register select bit */

/* I2C Functions */
void I2C_Init(void) {
    TWSR = 0x00;              /* Set prescaler bits to zero */
    TWBR = 0x46;              /* SCL frequency = 50kHz for F_CPU = 8MHz */
    TWCR = (1<<TWEN);         /* Enable TWI */
}

void I2C_Start(void) {
    TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);  /* Enable TWI, generate start condition */
    while (!(TWCR & (1<<TWINT)));            /* Wait for TWINT flag to set */
}

void I2C_Stop(void) {
    TWCR = (1<<TWINT)|(1<<TWSTO)|(1<<TWEN);  /* Enable TWI, generate stop condition */
    _delay_us(100);
}

uint8_t I2C_Write(uint8_t data) {
    TWDR = data;              /* Copy data to TWI data register */
    TWCR = (1<<TWINT)|(1<<TWEN); /* Enable TWI and clear interrupt flag */
    while (!(TWCR & (1<<TWINT)));  /* Wait for TWINT flag to set */
    
    // Check status
    uint8_t status = TWSR & 0xF8;
    if (status == 0x28 || status == 0x18) { /* 0x28 = TW_MT_DATA_ACK, 0x18 = TW_MT_SLA_ACK */
        return 0;  // ACK received
    } else {
        return 1;  // NACK or error
    }
}

/* LCD Functions */
void LCD_EnablePulse(uint8_t data) {
    I2C_Write(data | ENABLE);  /* Enable bit high */
    _delay_us(1);              /* Enable pulse width */
    I2C_Write(data & ~ENABLE); /* Enable bit low */
    _delay_ms(2);              /* Wait for the command to execute */
}

void LCD_Command(uint8_t cmnd) {
    uint8_t highNibble = (cmnd & 0xF0) | LCD_BACKLIGHT;
    uint8_t lowNibble = ((cmnd << 4) & 0xF0) | LCD_BACKLIGHT;

    I2C_Start();
    I2C_Write(LCD_I2C_ADDRESS << 1);  /* Send the I2C address with write mode */
    
    LCD_EnablePulse(highNibble);      /* Send the upper nibble */
    LCD_EnablePulse(lowNibble);       /* Send the lower nibble */
    
    I2C_Stop();
}

void LCD_Char(uint8_t data) {
    uint8_t highNibble = (data & 0xF0) | REGISTER_SELECT | LCD_BACKLIGHT;
    uint8_t lowNibble = ((data << 4) & 0xF0) | REGISTER_SELECT | LCD_BACKLIGHT;

    I2C_Start();
    I2C_Write(LCD_I2C_ADDRESS << 1);  /* Send the I2C address with write mode */
    
    LCD_EnablePulse(highNibble);      /* Send the upper nibble with RS=1 for data */
    LCD_EnablePulse(lowNibble);       /* Send the lower nibble with RS=1 */
    
    I2C_Stop();
}

void LCD_Init(void) {
    I2C_Init();                /* Initialize I2C */
    _delay_ms(20);             /* LCD Power ON delay */
    
    LCD_Command(0x02);         /* Initialize for 4-bit mode */
    LCD_Command(0x28);         /* 2 lines, 5x7 matrix in 4-bit mode */
    LCD_Command(0x0C);         /* Display ON, Cursor OFF */
    LCD_Command(0x06);         /* Auto increment cursor */
    LCD_Command(0x01);         /* Clear display */
    _delay_ms(2);
}

void LCD_SetCursor(uint8_t row, uint8_t col) {
    uint8_t address;
    if (row == 0) {
        address = 0x80 + col; // Move to column of row 0
    } else {
        address = 0xC0 + col; // Move to column of row 1
    }
    LCD_Command(address);  // Send command to move the cursor
}

void LCD_String(char *str) {
    while (*str) {
        LCD_Char(*str++);
    }
}

void LCD_Clear(void) {
    LCD_Command(0x01);  // 0x01 is the command to clear the display
    _delay_ms(2);       // Wait for the LCD to clear
}

#endif

