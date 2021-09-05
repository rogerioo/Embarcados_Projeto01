#include "pwm.hpp"

PWM::PWM(int pin)
{
    int res;

    this->pin = pin;

    res = wiringPiSetup();
    if (res != 0)
        throw "Couldn't setup pin mode.";

    pinMode(this->pin, OUTPUT);

    res = softPwmCreate(this->pin, 0, 100);
    if (res == -1)
        throw "Couldn't open PWM pin.";
}

PWM::~PWM()
{
    this->send(0);
    softPwmStop(this->pin);
}

void PWM::send(int value)
{
    if (value > 0)
        softPwmWrite(this->pin == FAN ? RESISTOR : FAN, 0);

    softPwmWrite(this->pin, value);
}
