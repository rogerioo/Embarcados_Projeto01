#ifndef SCREEN_HPP

#define SCREEN_HPP

#include <curses.h>
#include <menu.h>
#include <string>
#include <stdlib.h>
#include <vector>

class Screen
{
private:
    WINDOW *box_external_temperature;
    WINDOW *box_internal_temperature;
    WINDOW *box_reference_temperature;
    WINDOW *box_control_mode;
    WINDOW *box_menu;
    WINDOW *box_status;

    int max_height, max_width;

public:
    Screen(/* args */);
    ~Screen();
};

#endif