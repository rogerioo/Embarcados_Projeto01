#include <iostream>

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#include "control.hpp"

using namespace std;

float potentiometer_temperature;
float user_temperature = -100;
float internal_temperature;
int key_state;

int main(int argc, char *argv[])
{
    Control *control = new Control();
    // Display *display = new Display();
    // display_message data;

    // data.message = "T: ";
    // data.data = (const void *)&sensor_out->temperature;
    // display->write_lcd(LINE1, FLOAT_MSG, &data);

    // data.message = " P: ";
    // data.data = (const void *)&sensor_out->pressure;
    // display->write_lcd(0, FLOAT_MSG, &data);

    // data.message = "H:  ";
    // data.data = (const void *)&sensor_out->humidity;
    // display->write_lcd(LINE2, FLOAT_MSG, &data);
    // data.message = "%";
    // display->write_lcd(0, CHAR_MSG, &data);

    control->go();

    return 0;
}