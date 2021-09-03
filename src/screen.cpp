#include "screen.hpp"

#include <string.h>

using namespace std;

Screen::Screen() : box_external_temperature(nullptr),
                   box_internal_temperature(nullptr),
                   box_reference_temperature(nullptr),
                   box_control_mode(nullptr),
                   box_menu(nullptr),
                   box_status(nullptr)
{
    initscr();
    noecho();
    curs_set(0);

    vector<string> menu_options{
        "Option 01",
        "Option 02",
        "Option 03"};

    getmaxyx(stdscr, max_height, max_width);

    box_external_temperature = newwin(5, (max_width / 4) - 1, 0, 0);
    box_internal_temperature = newwin(5, (max_width / 4) - 1, 0, max_width / 4);
    box_reference_temperature = newwin(5, (max_width / 4) - 1, 0, (max_width / 4) * 2);
    box_control_mode = newwin(5, (max_width / 4) - 1, 0, (max_width / 4) * 3);

    box_menu = newwin(this->max_height - 6, this->max_width / 2 - 1, 6, 0);
    box_status = newwin(this->max_height - 6, this->max_width / 2 - 1, 6, this->max_width / 2);

    box(box_external_temperature, 0, 0);
    box(box_internal_temperature, 0, 0);
    box(box_reference_temperature, 0, 0);
    box(box_control_mode, 0, 0);

    box(box_menu, 0, 0);
    box(box_status, 0, 0);

    mvwprintw(box_external_temperature, 0, (max_width / 4) / 2 - strlen("EXTERNAL TEMPERATURE") / 2, "EXTERNAL TEMPERATURE");
    mvwprintw(box_internal_temperature, 0, (max_width / 4) / 2 - strlen("INTERNAL TEMPERATURE") / 2, "INTERNAL TEMPERATURE");
    mvwprintw(box_reference_temperature, 0, (max_width / 4) / 2 - strlen("REFERENCE TEMPERATURE") / 2, "REFERENCE TEMPERATURE");
    mvwprintw(box_control_mode, 0, (max_width / 4) / 2 - strlen("CONTROL MODE") / 2, "CONTROL MODE");

    mvwprintw(box_menu, 0, (max_width / 2) / 2 - strlen("MENU") / 2, "MENU");
    mvwprintw(box_status, 0, (max_width / 2) / 2 - strlen("DEVICE STATUS") / 2, "DEVICE STATUS");

    wrefresh(box_external_temperature);
    wrefresh(box_internal_temperature);
    wrefresh(box_reference_temperature);
    wrefresh(box_control_mode);

    wrefresh(box_menu);
    wrefresh(box_status);
}

Screen::~Screen()
{
    delwin(box_external_temperature);
    delwin(box_internal_temperature);
    delwin(box_reference_temperature);
    delwin(box_control_mode);
    delwin(box_menu);
    delwin(box_status);

    endwin();
}