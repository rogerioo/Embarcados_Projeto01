#ifndef SCREEN_HPP

#define SCREEN_HPP

#define ENTER_KEY 10

#include <curses.h>
#include <menu.h>
#include <string>
#include <stdlib.h>
#include <vector>
#include <signal.h>
#include <unistd.h>

using namespace std;

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

    int max_height, max_width;

    void set_menu();
    void set_header();
    void set_status();

public:
    Screen();
    ~Screen();

    void menu_deamon();
};

#endif