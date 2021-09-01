#include "pwm.hpp"

PWM::PWM(int pin)
{
    this->pin = pin;

    wiringPiSetup();
    pinMode(this->pin, OUTPUT);
    softPwmCreate(this->pin, 0, 100);
}

PWM::~PWM()
{
    softPwmStop(this->pin);
}

void PWM::send(int value)
{
    if (value > 0)
        softPwmWrite(this->pin, 0);

    softPwmWrite(this->pin, value);
}
