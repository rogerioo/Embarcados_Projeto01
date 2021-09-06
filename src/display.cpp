#include "display.hpp"

using namespace std;

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
  clear_lcd();
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

void Display::update_display_deamon()
{
  display_message data;

  while (not abort_deamon)
  {
    auto reference_temperature = user_temperature == -100 ? potentiometer_temperature : user_temperature;

    auto current_key_state = user_key_state == -1 ? key_state : user_key_state;
    string key_state_message{current_key_state == 0 ? " ON/OFF" : " PID   "};

    data.message = "TE:";
    data.data = (const void *)&external_temperature;
    write_lcd(LINE1, FLOAT_MSG, &data);

    data.message = " TI:";
    data.data = (const void *)&internal_temperature;
    write_lcd(0, FLOAT_MSG, &data);

    data.message = "TR:";
    data.data = (const void *)&reference_temperature;
    write_lcd(LINE2, FLOAT_MSG, &data);

    data.message = key_state_message.c_str();
    write_lcd(0, CHAR_MSG, &data);

    sleep(1);
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
