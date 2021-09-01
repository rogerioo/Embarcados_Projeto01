#ifndef PMW_HPP
#define PMW_HPP

#include <softPwm.h>
#include <wiringPi.h>

#define RESISTOR 4
#define FAN 5

class PWM
{
private:
    int pin;

public:
    PWM(int pin);
    ~PWM();

    void send(int value);
};

#endif