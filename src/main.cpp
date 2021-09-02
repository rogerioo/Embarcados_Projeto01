#include <iostream>

#include <signal.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#include "control.hpp"

using namespace std;

float potentiometer_temperature = 0;
float user_temperature = -100;
float internal_temperature = 0;
float external_temperature = 0;
int key_state = -1;

Control *control = new Control();

void quit(int signal)
{
    cout << "Killing system" << endl;

    delete control;

    exit(0);
}

int main(int argc, char *argv[])
{

    signal(SIGINT, quit);

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