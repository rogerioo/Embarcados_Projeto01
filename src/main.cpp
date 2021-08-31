//
// BME280 sample app
//
#define USE_OLED

#include <stdio.h>
#include <unistd.h>

#include "display.hpp"
#include "bme280.hpp"

#define DELAY 1000000

int main(int argc, char *argv[])
{
    Display *display = new Display();
    display_message data;

    BME280 *sensor = new BME280(1, 0x76);
    sensor_data *response = sensor->get_data();

    data.message = "T: ";
    data.data = (const void *)&response->temperature;
    display->write_lcd(LINE1, FLOAT_MSG, &data);

    data.message = " P: ";
    data.data = (const void *)&response->pressure;
    display->write_lcd(0, FLOAT_MSG, &data);

    data.message = "H:  ";
    data.data = (const void *)&response->humidity;
    display->write_lcd(LINE2, FLOAT_MSG, &data);
    data.message = "%";
    display->write_lcd(0, CHAR_MSG, &data);

    return 0;
}