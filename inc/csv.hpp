#ifndef CSV_HPP

#define CSV_HPP

#include <fstream>

using namespace std;

class CSV
{
private:
    fstream file;

public:
    CSV();
    ~CSV();

    void write_line(float internal_temperature, float external_temperature, float reference_temperature, int resistor, int fan);
};

#endif // CSV_HPP
