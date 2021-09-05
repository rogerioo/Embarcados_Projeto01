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
                                 "2. Modify Temperature",
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
    curs_set(false);

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
    set_menu_sub(menu_controller, subwin(box_menu, 6, (max_width / 2) - 4, 8, 1));

    /* Set menu mark to the string " * " */
    set_menu_mark(menu_controller, " > ");

    /* Post the menu */
    post_menu(menu_controller);
}

void Screen::menu_deamon()
{

    // wrefresh(box_menu);

    int user_input;

    while (not abort_deamon)
    {
        user_input = wgetch(box_menu);
        switch (user_input)
        {
        case KEY_DOWN:
            menu_driver(menu_controller, REQ_DOWN_ITEM);
            break;
        case KEY_UP:
            menu_driver(menu_controller, REQ_UP_ITEM);
            break;
        case ENTER_KEY:
            auto selected_item = item_name(current_item(menu_controller));

            switch (*selected_item)
            {
            case '1':
                set_input_mode({"Enter the Control Strategy:",
                                "",
                                "(0) ON/OFF",
                                "",
                                "(1) PID"},
                               1);
                break;
            case '2':
                set_input_mode({"Enter the Temperature: "}, 2);
                break;
            case '3':
                set_input_mode({"Enter the Hysterese: "}, 3);
                break;
            case '4':
                set_input_mode({"Enter the PID parameters "}, 4);
                break;

            case '5':
                kill(getpid(), SIGINT);
                break;
            }

            break;
        }
        wrefresh(box_menu);
    }
}

void Screen::set_input_mode(vector<string> message, int option)
{
    int window_height, window_width;
    int back_to_auto;

    getmaxyx(box_menu, window_height, window_width);

    int begin_y = window_height - 1 - message.size() - 2;

    echo();

    mvwprintw(box_menu, begin_y + message.size() + 1, 3, "Want to set it auto? (1 Yes / 2 No): ");
    wrefresh(box_menu);

    wscanw(box_menu, "%d", &back_to_auto);

    mvwprintw(box_menu, begin_y + message.size() + 1, 3, "                                      ");
    wrefresh(box_menu);

    if (back_to_auto == 1)
    {
        switch (option)
        {
        case 1:
            user_key_state = -1;
            break;
        case 2:
            user_key_state = -100;
            break;
        case 3:
            user_key_state = -1;
            break;
        case 4:
            user_pid_ki = -1;
            user_pid_kd = -1;
            user_pid_kp = -1;

            break;
        default:
            break;
        }

        noecho();

        return;
    }

    for (size_t i = 0; i < message.size(); i++)
    {
        if (message[i] == "")
            continue;
        mvwprintw(box_menu, begin_y + i, 3, message[i].c_str());
    }

    wrefresh(box_menu);

    switch (option)
    {
    case 1:
        wscanw(box_menu, "%d", &user_key_state);
        break;
    case 2:
        wscanw(box_menu, "%f", &user_temperature);
        break;
    case 3:
        wscanw(box_menu, "%d", &user_hysteresis);
        break;
    case 4:
        mvwprintw(box_menu, begin_y + message.size() + 1, 3, "Enter the KP: ");
        wrefresh(box_menu);
        wscanw(box_menu, "%d", &user_pid_kp);

        mvwprintw(box_menu, begin_y + message.size() + 1, 3, "               ");
        wrefresh(box_menu);
        mvwprintw(box_menu, begin_y + message.size() + 1, 3, "Enter the KI: ");
        wrefresh(box_menu);
        wscanw(box_menu, "%d", &user_pid_kp);

        mvwprintw(box_menu, begin_y + message.size() + 1, 3, "               ");
        wrefresh(box_menu);
        mvwprintw(box_menu, begin_y + message.size() + 1, 3, "Enter the KD: ");
        wrefresh(box_menu);
        wscanw(box_menu, "%d", &user_pid_kp);

        break;
    default:
        break;
    }

    noecho();

    string clear_string(window_width - 2, ' ');

    for (size_t i = 0; i < message.size() + 2; i++)
    {
        mvwprintw(box_menu, begin_y + i, 1, clear_string.c_str());
    }

    wrefresh(box_menu);
}

void Screen::data_update_deamon()
{
    int box_control_mode_height, box_control_mode_width;
    int box_reference_height, box_reference_width;
    int box_external_height, box_external_width;
    int box_internal_height, box_internal_width;

    getmaxyx(box_reference_temperature, box_reference_height, box_reference_width);
    getmaxyx(box_control_mode, box_control_mode_height, box_control_mode_width);
    getmaxyx(box_external_temperature, box_external_height, box_external_width);
    getmaxyx(box_internal_temperature, box_internal_height, box_internal_width);

    while (not abort_deamon)
    {
        mvwprintw(box_external_temperature, 2, box_external_width / 2, "%.2f", external_temperature);
        mvwprintw(box_internal_temperature, 2, box_internal_width / 2, "%.2f", internal_temperature);
        mvwprintw(box_reference_temperature, 2, box_reference_width / 2, "%.2f", user_temperature == -100 ? potentiometer_temperature : user_temperature);
        mvwprintw(box_control_mode, 2, box_control_mode_width / 2, "%d", user_key_state == -1 ? key_state : user_key_state);

        wrefresh(box_external_temperature);
        wrefresh(box_internal_temperature);
        wrefresh(box_reference_temperature);
        wrefresh(box_control_mode);

        sleep(1);
    }
}