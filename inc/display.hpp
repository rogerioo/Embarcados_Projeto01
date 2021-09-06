#ifndef DISPLAY_HPP
#define DISPLAY_HPP

#define I2C_ADDR 0x27 // I2C device address

// Define some device constants
#define LCD_CHR 1 // Mode - Sending data
#define LCD_CMD 0 // Mode - Sending command

#define LINE1 0x80 // 1st line
#define LINE2 0xC0 // 2nd line

#define LCD_BACKLIGHT 0x08 // On
// LCD_BACKLIGHT = 0x00  # Off

#define ENABLE 0b00000100 // Enable bit

#define CHAR_MSG 1
#define INT_MSG 2
#define FLOAT_MSG 3

#include <wiringPiI2C.h>
#include <wiringPi.h>
#include <unistd.h>

#include <string>

extern float potentiometer_temperature;
extern float internal_temperature;
extern float external_temperature;
extern int key_state;

extern float user_temperature;
extern int user_key_state;

extern bool abort_deamon;

struct display_message
{
    const void *data;
    const char *message;
} typedef display_message;

class Display
{
private:
    int display_fd;

    void lcd_byte(int bits, int mode);
    void lcd_toggle_enable(int bits);

    void typeInt(int i);
    void typeFloat(float myFloat);
    void lcdLoc(int line);
    void typeln(const char *s);
    void typeChar(char val);

    /*!
    *  @brief Writes on the LCD display
    *
    *  @param[in] line     : Which line to use | LINE1 or LINE2
    *  @param[in] type     : Which data type to write | 1: char array, 2: int or 3: float
    *  @param[in] data     : Data to be shown
    *
    */
    void write_lcd(int line, int type, display_message *data);

    /*!
    *  @brief Clean the display
    */
    void clear_lcd();

public:
    Display();
    ~Display();

    void update_display_deamon();
};

#endif