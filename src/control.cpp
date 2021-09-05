#include <iostream>

#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

#include "control.hpp"

using namespace std;

Control::Control()
{
    try
    {
        this->uart = new UART("/dev/serial0", O_RDWR | O_NOCTTY | O_NDELAY, 0x01);

        this->sensor = new BME280(1, 0x76);

        this->resistor = new PWM(RESISTOR);
        this->fan = new PWM(FAN);

        this->csv = new CSV();
    }
    catch (const char *error)
    {
        throw error;
    }

    this->pid = new PID(5.0, 1.0, 5.0);
    this->last_on_off = 0;
}

Control::~Control()
{
    delete this->resistor;
    delete this->sensor;
    delete this->uart;
    delete this->fan;
    delete this->pid;
    delete this->csv;
}

void Control::set_potentiometer_temperature()
{
    data_interface request, response;

    request.sub_code = READ_TR;
    request.code = READ;

    try
    {
        uart->send_message(&request);
        uart->read_message(&response);
    }
    catch (const char *error)
    {
        UART_status = false;
        throw error;
    }

    UART_status = true;

    potentiometer_temperature = (*((float *)response.data));

    free(response.data);
}

void Control::set_internal_temperature()
{
    data_interface request, response;

    request.code = READ;
    request.sub_code = READ_TI;

    try
    {
        uart->send_message(&request);
        uart->read_message(&response);
    }
    catch (const char *error)
    {
        UART_status = false;
        throw error;
    }

    UART_status = true;

    internal_temperature = (*((float *)response.data));

    free(response.data);
}

void Control::set_external_temperature()
{
    sensor_data *sensor_out = this->sensor->get_data();

    external_temperature = sensor_out->temperature;
}

void Control::set_key_state()
{
    data_interface request, response;

    request.code = READ;
    request.sub_code = READ_STATE;

    try
    {
        uart->send_message(&request);
        uart->read_message(&response);
    }
    catch (const char *error)
    {
        UART_status = false;
        throw error;
    }

    UART_status = true;

    key_state = (*((int *)response.data));

    free(response.data);
}

void Control::send_control_signal(int control_signal)
{
    data_interface request;

    request.code = WRITE;
    request.sub_code = WRITE_SIGNAL;

    request.data = (int *)malloc(sizeof(int));

    *((int *)request.data) = control_signal;

    try
    {
        uart->send_message(&request);
    }
    catch (char const *error)
    {
        UART_status = false;
        throw error;
    }

    UART_status = true;

    free(request.data);
}

int Control::set_pid_control_signal(float reference_temperature)
{
    this->pid->set_reference(reference_temperature);

    int control_signal = (int)this->pid->get_pid(internal_temperature);

    if (control_signal >= 0)
        this->resistor->send(control_signal);

    else if (control_signal <= -40)
        this->fan->send(-control_signal);

    else
    {
        this->resistor->send(0);
        this->fan->send(0);
    }

    return control_signal;
}

int Control::set_on_off_signal(float reference_temperature)
{
    int control_signal = this->last_on_off;

    auto current_hysteresis = user_hysteresis == -1 ? hysteresis : user_hysteresis;

    float bottom = reference_temperature - current_hysteresis / 2.0;
    float top = reference_temperature + current_hysteresis / 2.0;

    if (internal_temperature >= top)
        control_signal = -100, this->fan->send(100);
    else if (internal_temperature <= bottom)
        control_signal = 100, this->resistor->send(100);

    this->last_on_off = control_signal;

    return control_signal;
}

void Control::go()
{
    while (not abort_deamon)
    {
        this->set_potentiometer_temperature();
        this->set_internal_temperature();
        this->set_external_temperature();
        this->set_key_state();

        float reference_temperature = user_temperature == -100 ? potentiometer_temperature : user_temperature;
        auto current_key_state = user_key_state == -1 ? key_state : user_key_state;

        int control_signal;

        if (current_key_state == CONTROL_ON_OFF)
            control_signal = this->set_on_off_signal(reference_temperature);
        else if (current_key_state == CONTROL_PID)
        {
            control_signal = this->set_pid_control_signal(reference_temperature);
            this->last_on_off = 0;
        }

        this->send_control_signal(control_signal);

        this->csv->write_line(internal_temperature, external_temperature, reference_temperature, control_signal);

        sleep(1);
    }
}
