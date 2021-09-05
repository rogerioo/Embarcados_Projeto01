#include <iostream>

#include <signal.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <thread>

#include "control.hpp"
#include "screen.hpp"

using namespace std;

bool abort_deamon = false;

float user_temperature = -100;
int user_hysteresis = -1;
int user_key_state = -1;
int user_pid_kp = -1;
int user_pid_ki = -1;
int user_pid_kd = -1;

float potentiometer_temperature = 0;
float internal_temperature = 0;
float external_temperature = 0;
int hysteresis = 3;
int key_state = -1;

bool RESISTOR_status = true;
bool BME280_status = true;
bool UART_status = true;
bool LCD_display = true;
bool FAN_status = true;

Control *control = new Control();
Screen *screen = new Screen();

thread *screen_update_thread;
thread *screen_menu_thread;

void quit(int signal)
{
    abort_deamon = true;

    screen_update_thread->join();
    screen_menu_thread->join();

    delete screen_update_thread;
    delete screen_menu_thread;

    delete screen;

    delete control;

    exit(0);
}

int main(int argc, char *argv[])
{

    signal(SIGINT, quit);

    screen_update_thread = new thread(&Screen::data_update_deamon, screen);
    screen_menu_thread = new thread(&Screen::menu_deamon, screen);

    // screen->data_update_deamon();

    // Screen screen;

    while (1)
    {
    }

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

    // control->go();

    return 0;
}