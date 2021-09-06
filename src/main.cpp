#include <iostream>
#include <thread>

#include <signal.h>

#include "control.hpp"
#include "display.hpp"
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
int key_state = 0;

bool RESISTOR_status = true;
bool BME280_status = true;
bool UART_status = true;
bool LCD_display = true;
bool FAN_status = true;

Control *control;
Display *display;
Screen *screen;

thread *screen_update_thread;
thread *screen_menu_thread;

thread *control_thread;

thread *display_thread;

void quit(int signal)
{
    abort_deamon = true;

    control_thread->join();

    screen_update_thread->join();
    screen_menu_thread->join();

    display_thread->join();

    delete control_thread;

    delete screen_update_thread;
    delete screen_menu_thread;

    delete display_thread;

    delete screen;

    delete control;

    delete display;

    exit(0);
}

int main(int argc, char *argv[])
{

    signal(SIGINT, quit);

    try
    {
        control = new Control();
        screen = new Screen();
        display = new Display();
    }
    catch (const char *error)
    {
        cout << error << endl;
        return -1;
    }

    screen_update_thread = new thread(&Screen::data_update_deamon, screen);
    screen_menu_thread = new thread(&Screen::menu_deamon, screen);

    control_thread = new thread(&Control::go, control);

    display_thread = new thread(&Display::update_display_deamon, display);

    while (1)
    {
    }

    return 0;
}