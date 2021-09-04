#ifndef SCREEN_HPP

#define SCREEN_HPP

#define ENTER_KEY 10

#include <curses.h>
#include <menu.h>
#include <string>
#include <stdlib.h>
#include <vector>
#include <utility>
#include <algorithm>
#include <signal.h>
#include <unistd.h>

using namespace std;

extern float potentiometer_temperature;
extern float internal_temperature;
extern float external_temperature;
extern float user_temperature;
extern int user_hysteresis;
extern int user_key_state;
extern int user_pid_kp;
extern int user_pid_ki;
extern int user_pid_kd;
extern int hysteresis;
extern int key_state;

extern bool RESISTOR_status;
extern bool BME280_status;
extern bool UART_status;
extern bool LCD_display;
extern bool FAN_status;

class Screen
{
private:
    WINDOW *box_external_temperature;
    WINDOW *box_internal_temperature;
    WINDOW *box_reference_temperature;
    WINDOW *box_control_mode;
    WINDOW *box_menu;
    WINDOW *box_status;

    MENU *menu_controller;
    ITEM **menu_items;

    vector<string> menu_options;
    vector<pair<string, bool>> system_devices;

    int max_height, max_width;

    void set_menu();
    void set_header();
    void set_status();

    void set_input_mode(vector<string> message, int option);

public:
    Screen();
    ~Screen();

    void menu_deamon();
};

#endif