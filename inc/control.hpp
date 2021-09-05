#ifndef CONTROL_HPP

#define CONTROL_HPP

#include "bme280.hpp"
#include "uart.hpp"
#include "pwm.hpp"
#include "pid.hpp"
#include "csv.hpp"

#define CONTROL_ON_OFF 0
#define CONTROL_PID 1

extern bool abort_deamon;

extern float potentiometer_temperature;
extern float internal_temperature;
extern float external_temperature;
extern int hysteresis;
extern int key_state;

extern float user_temperature;
extern int user_hysteresis;
extern int user_key_state;
extern int user_pid_kp;
extern int user_pid_ki;
extern int user_pid_kd;

extern bool RESISTOR_status;
extern bool BME280_status;
extern bool UART_status;
extern bool FAN_status;

class Control
{
private:
    BME280 *sensor;
    PWM *resistor;
    UART *uart;
    PWM *fan;
    PID *pid;
    CSV *csv;

    int last_on_off;

    void send_control_signal(int control_signal);
    void set_potentiometer_temperature();
    void set_internal_temperature();
    void set_external_temperature();
    void set_key_state();

    int set_pid_control_signal(float reference_temperature);
    int set_on_off_signal(float reference_temperature);

public:
    Control();
    ~Control();

    void go();
};

#endif
