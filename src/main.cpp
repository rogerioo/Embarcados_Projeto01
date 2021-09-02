#include <iostream>

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#include "display.hpp"
#include "bme280.hpp"
#include "uart.hpp"
#include "pwm.hpp"
#include "pid.hpp"

#define DELAY 1000000

using namespace std;

int main(int argc, char *argv[])
{
    float internal_temperature, reference_temperature;
    char student_id[] = {1, 7, 5, 1};

    UART *uart = new UART("/dev/serial0", O_RDWR | O_NOCTTY | O_NDELAY, 0x01, student_id);

    Display *display = new Display();
    display_message data;

    BME280 *sensor = new BME280(1, 0x76);
    sensor_data *sensor_out = sensor->get_data();

    PWM *resistor = new PWM(RESISTOR);
    PWM *fan = new PWM(FAN);

    PID *pid = new PID(5.0, 1.0, 5.0);

    // resistor->send(100);
    fan->send(100);
    sleep(60);

    data.message = "T: ";
    data.data = (const void *)&sensor_out->temperature;
    display->write_lcd(LINE1, FLOAT_MSG, &data);

    data.message = " P: ";
    data.data = (const void *)&sensor_out->pressure;
    display->write_lcd(0, FLOAT_MSG, &data);

    data.message = "H:  ";
    data.data = (const void *)&sensor_out->humidity;
    display->write_lcd(LINE2, FLOAT_MSG, &data);
    data.message = "%";
    display->write_lcd(0, CHAR_MSG, &data);

    data_interface request, response;

    request.code = READ;
    request.sub_code = READ_TI;

    uart->send_message(&request);
    uart->read_message(&response);

    cout << "\nTI:" << endl;
    cout << (*((float *)response.data)) << endl;

    internal_temperature = (*((float *)response.data));

    free(response.data);

    request.code = READ;
    request.sub_code = READ_TR;

    uart->send_message(&request);
    uart->read_message(&response);

    cout << "\nTR:" << endl;
    cout << (*((float *)response.data)) << endl;

    reference_temperature = (*((float *)response.data));

    free(response.data);

    request.code = READ;
    request.sub_code = READ_STATE;

    uart->send_message(&request);
    uart->read_message(&response);

    cout << "\nState:" << endl;
    cout << (*((float *)response.data)) << endl;

    free(response.data);

    request.code = WRITE;
    request.sub_code = WRITE_SIGNAL;

    request.data = (int *)malloc(sizeof(int));

    *((int *)request.data) = -21;

    uart->send_message(&request);

    free(request.data);

    pid->set_reference(reference_temperature);

    printf("PID: %.0lf\n", pid->get_pid(internal_temperature));

    delete fan;
    delete resistor;

    return 0;
}