#include <ctime>
#include <sstream>
#include <iostream>
#include <iomanip>
#include "csv.hpp"

using namespace std;

string get_time_now(const char *separator, const char *time_separator)
{
    stringstream str;

    const time_t dt_hour = time(0);
    tm *now = localtime(&dt_hour);

    str << setw(2) << setfill('0') << now->tm_year + 1900
        << "-"
        << setw(2) << setfill('0') << now->tm_mon + 1
        << "-"
        << setw(2) << setfill('0') << now->tm_mday
        << separator
        << setw(2) << setfill('0') << now->tm_hour
        << time_separator
        << setw(2) << setfill('0') << now->tm_min
        << time_separator
        << setw(2) << setfill('0') << now->tm_sec;

    return str.str();
}

CSV::CSV()
{
    stringstream filename;

    filename << "logs/control_log_"
             << get_time_now("_", "-")
             << ".csv";

    this->file.open(filename.str(), ios::out | ios::app);

    if (not filename)
        throw "Could not open CSV file.";
}

CSV::~CSV()
{
    this->file.close();
}

void CSV::write_line(float internal_temperature, float external_temperature, float reference_temperature, int control_signal)
{
    this->file << get_time_now(" ", ":") << ","
               << internal_temperature << ","
               << external_temperature << ","
               << reference_temperature << ","
               << control_signal << endl;
}