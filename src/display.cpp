#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <stdlib.h>
#include <stdio.h>

#include "display.hpp"

Display::Display()
{
  if (wiringPiSetup() == -1)
    exit(1);

  this->display_fd = wiringPiI2CSetup(I2C_ADDR);

  this->lcd_byte(0x33, LCD_CMD); // Initialise
  this->lcd_byte(0x32, LCD_CMD); // Initialise
  this->lcd_byte(0x06, LCD_CMD); // Cursor move direction
  this->lcd_byte(0x0C, LCD_CMD); // 0x0F On, Blink Off
  this->lcd_byte(0x28, LCD_CMD); // Data length, number of lines, font size
  this->lcd_byte(0x01, LCD_CMD); // Clear display

  delayMicroseconds(500);
}

Display::~Display()
{
}

void Display::write_lcd(int line, int type, display_message *data)
{
  if (line == LINE1 || line == LINE2)
    this->lcdLoc(line);

  switch (type)
  {
  case 1:
    this->typeln(data->message);
    break;
  case 2:
    this->typeln(data->message);
    this->typeInt(*((int *)data->data));
    break;
  case 3:
    this->typeln(data->message);
    this->typeFloat(*((float *)data->data));
    break;

  default:
    throw "Wrong option";
    break;
  }
}

void Display::lcd_byte(int bits, int mode)
{

  //Send byte to data pins
  // bits = the data
  // mode = 1 for data, 0 for command
  int bits_high;
  int bits_low;
  // uses the two half byte writes to LCD
  bits_high = mode | (bits & 0xF0) | LCD_BACKLIGHT;
  bits_low = mode | ((bits << 4) & 0xF0) | LCD_BACKLIGHT;

  // High bits
  wiringPiI2CReadReg8(this->display_fd, bits_high);
  lcd_toggle_enable(bits_high);

  // Low bits
  wiringPiI2CReadReg8(this->display_fd, bits_low);
  lcd_toggle_enable(bits_low);
}

// float to string
void Display::typeFloat(float myFloat)
{
  char buffer[20];
  sprintf(buffer, "%0.1f", myFloat);
  typeln(buffer);
}

// int to string
void Display::typeInt(int i)
{
  char array1[20];
  sprintf(array1, "%d", i);
  typeln(array1);
}

// clr lcd go home loc 0x80
void Display::clear_lcd()
{
  this->lcd_byte(0x01, LCD_CMD);
  this->lcd_byte(0x02, LCD_CMD);
}

// go to location on LCD
void Display::lcdLoc(int line)
{
  this->lcd_byte(line, LCD_CMD);
}

// out char to LCD at current position
void Display::typeChar(char val)
{

  this->lcd_byte(val, LCD_CHR);
}

// this allows use of any size string
void Display::typeln(const char *s)
{

  while (*s)
    this->lcd_byte(*(s++), LCD_CHR);
}

void Display::lcd_toggle_enable(int bits)
{
  // Toggle enable pin on LCD display
  delayMicroseconds(500);
  wiringPiI2CReadReg8(this->display_fd, (bits | ENABLE));
  delayMicroseconds(500);
  wiringPiI2CReadReg8(this->display_fd, (bits & ~ENABLE));
  delayMicroseconds(500);
}
