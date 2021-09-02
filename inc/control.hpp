#ifndef CONTROL_HPP

#define CONTROL_HPP

#include "bme280.hpp"
#include "uart.hpp"
#include "pwm.hpp"
#include "pid.hpp"
#include "csv.hpp"

extern float potentiometer_temperature;
extern float internal_temperature;
extern float user_temperature;
extern int key_state;

class Control
{
private:
    BME280 *sensor;
    PWM *resistor;
    UART *uart;
    PWM *fan;
    PID *pid;
    CSV *csv;

    void send_control_signal(int control_signal);
    void set_potentiometer_temperature();
    void set_internal_temperature();
    void set_key_state();

public:
    Control();
    ~Control();

    void go();
};

#endif
