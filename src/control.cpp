#include <iostream>

#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

#include "control.hpp"

using namespace std;

Control::Control()
{
    this->uart = new UART("/dev/serial0", O_RDWR | O_NOCTTY | O_NDELAY, 0x01);

    this->sensor = new BME280(1, 0x76);

    this->resistor = new PWM(RESISTOR);
    this->fan = new PWM(FAN);

    this->pid = new PID(5.0, 1.0, 5.0);

    this->csv = new CSV();
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

    this->uart->send_message(&request);
    this->uart->read_message(&response);

    potentiometer_temperature = (*((float *)response.data));

    free(response.data);
}

void Control::set_internal_temperature()
{
    data_interface request, response;

    request.code = READ;
    request.sub_code = READ_TI;

    this->uart->send_message(&request);
    this->uart->read_message(&response);

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

    uart->send_message(&request);
    uart->read_message(&response);

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

    uart->send_message(&request);

    free(request.data);
}

void Control::go()
{
    while (1)
    {
        this->set_potentiometer_temperature();
        this->set_internal_temperature();
        this->set_external_temperature();
        this->set_key_state();

        float reference_temperature = user_temperature == -100 ? potentiometer_temperature : user_temperature;

        this->pid->set_reference(reference_temperature);

        double control_signal = this->pid->get_pid(internal_temperature);

        this->send_control_signal((int)control_signal);

        if (control_signal >= 0)
            this->resistor->send(control_signal);

        else if (control_signal <= -40)
            this->fan->send(-control_signal);

        else
        {
            this->resistor->send(0);
            this->fan->send(0);
        }

        this->csv->write_line(internal_temperature, external_temperature, reference_temperature, control_signal);

        cout << "TI: " << internal_temperature << endl;
        cout << "TR: " << reference_temperature << endl;
        cout << "CS: " << control_signal << endl;

        sleep(10);
    }
}
