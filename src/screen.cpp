#include "screen.hpp"

#include <string.h>
#include <iostream>

using namespace std;

Screen::Screen() : box_external_temperature(nullptr),
                   box_internal_temperature(nullptr),
                   box_reference_temperature(nullptr),
                   box_control_mode(nullptr),
                   box_menu(nullptr),
                   box_status(nullptr),
                   menu_controller(nullptr),
                   menu_items(nullptr),
                   menu_options({"1. Modify Control Strategy",
                                 "2. Modify Manual Temperature",
                                 "3. Modify Hysterese",
                                 "4. Modify PID Parameters",
                                 " ",
                                 "5. Shutdown System",
                                 ""}),
                   system_devices({
                       {"RESISTOR", RESISTOR_status},
                       {"BME280", BME280_status},
                       {"UART", UART_status},
                       {"DISPLAY", LCD_display},
                       {"FAN", FAN_status},
                   })
{
    initscr();
    noecho();
    curs_set(0);

    getmaxyx(stdscr, max_height, max_width);

    set_header();
    set_menu();
    set_status();

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

    unpost_menu(menu_controller);
    free_menu(menu_controller);

    endwin();
}

void Screen::set_header()
{
    string title_it{"INTERNAL TEMPERATURE"};
    string title_et{"EXTERNAL TEMPERATURE"};
    string title_rt{"REFERENCE TEMPERATURE"};
    string title_cm{"CONTROL MODE"};

    box_external_temperature = newwin(5, (max_width / 4) - 1, 0, 0);
    box_internal_temperature = newwin(5, (max_width / 4) - 1, 0, max_width / 4);
    box_reference_temperature = newwin(5, (max_width / 4) - 1, 0, (max_width / 4) * 2);
    box_control_mode = newwin(5, (max_width / 4) - 1, 0, (max_width / 4) * 3);

    box(box_external_temperature, 0, 0);
    box(box_internal_temperature, 0, 0);
    box(box_reference_temperature, 0, 0);
    box(box_control_mode, 0, 0);

    mvwprintw(box_external_temperature, 0, (max_width / 4) / 2 - title_et.size() / 2, title_et.c_str());
    mvwprintw(box_internal_temperature, 0, (max_width / 4) / 2 - title_it.size() / 2, title_it.c_str());
    mvwprintw(box_reference_temperature, 0, (max_width / 4) / 2 - title_rt.size() / 2, title_rt.c_str());
    mvwprintw(box_control_mode, 0, (max_width / 4) / 2 - title_cm.size() / 2, title_cm.c_str());
}

void Screen::set_status()
{
    string title{"DEVICES STATUS"};

    box_status = newwin(max_height - 6, max_width / 2 - 1, 6, max_width / 2);

    box(box_status, 0, 0);

    mvwprintw(box_status, 0, (max_width / 2) / 2 - title.size() / 2, title.c_str());

    auto right_padding = max_element(system_devices.begin(),
                                     system_devices.end(),
                                     [](const auto &lhs, const auto &rhs)
                                     { return lhs.first.length() < rhs.first.length(); })
                             ->first.length() +
                         3;

    int line_position = -(system_devices.size() / 2) - (max_height % 2 != 0);
    for (const auto item : system_devices)
    {
        mvwprintw(box_status, (max_height - 6) / 2 + line_position, 3, item.first.c_str());
        mvwprintw(box_status, (max_height - 6) / 2 + line_position++,
                  3 + item.first.length() + (right_padding - item.first.length()),
                  item.second ? "ACTIVE" : "ERROR");
    }
}

void Screen::set_menu()
{
    string right_padding((max_width / 2) - 4, ' ');
    string title{"MENU"};

    for (size_t i = 0; i < menu_options.size() - 1; i++)
        menu_options[i].append(right_padding);

    keypad(stdscr, TRUE);

    box_menu = newwin(max_height - 6, max_width / 2 - 1, 6, 0);

    box(box_menu, 0, 0);
    mvwprintw(box_menu, 0, (max_width / 2) / 2 - title.size() / 2, title.c_str());

    /* Create items */
    menu_items = (ITEM **)calloc(menu_options.size(), sizeof(ITEM *));
    for (size_t i = 0; i < menu_options.size(); ++i)
        menu_items[i] = new_item(menu_options[i].c_str(), "");

    item_opts_off(menu_items[4], O_SELECTABLE);
    item_opts_off(menu_items[5], O_SELECTABLE);

    /* Crate menu */
    menu_controller = new_menu((ITEM **)menu_items);

    /* Create the window to be associated with the menu */
    keypad(box_menu, TRUE);

    /* Set main window and sub window */
    set_menu_win(menu_controller, box_menu);
    set_menu_sub(menu_controller, subwin(box_menu, 6, (max_width / 2) - 4, (max_height / 2), 1));

    /* Set menu mark to the string " * " */
    set_menu_mark(menu_controller, " > ");

    /* Post the menu */
    post_menu(menu_controller);
}

void Screen::menu_deamon()
{

    // wrefresh(box_menu);

    int c;

    while ((c = wgetch(box_menu)) != 'a')
    {
        switch (c)
        {
        case KEY_DOWN:
            menu_driver(menu_controller, REQ_DOWN_ITEM);
            break;
        case KEY_UP:
            menu_driver(menu_controller, REQ_UP_ITEM);
            break;
        case ENTER_KEY:
            auto selected_item = item_name(current_item(menu_controller));

            mvwprintw(box_menu, 0, 1, "%s", selected_item);

            if (*selected_item == '5')
                // mvwprintw(box_menu, 0, 1, "%s", selected_item);
                kill(getpid(), SIGINT);

            break;
        }
        wrefresh(box_menu);
    }
}