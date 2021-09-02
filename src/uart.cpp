#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include "uart.hpp"
#include "crc.hpp"

using namespace std;

char student_id[4] = {1, 7, 5, 1};

void realloc_pointer(unsigned char *package, int *len, size_t bytes)
{
    unsigned char *tmp = package;

    *len += bytes;

    tmp = (unsigned char *)realloc(tmp, *len);
    package = tmp;

    return;
}

UART::UART(const char *path, int o_flag, int device_address)
{
    struct termios uart_config;

    uart_config.c_cflag = B9600 | CS8 | CLOCAL | CREAD;
    uart_config.c_iflag = IGNPAR;
    uart_config.c_oflag = 0;
    uart_config.c_lflag = 0;
    this->uart_device = -1;

    this->uart_device = open(path, o_flag);

    if (this->uart_device == -1)
        throw "Erro na abertura da UART";

    tcflush(this->uart_device, TCIFLUSH);
    tcsetattr(this->uart_device, TCSANOW, &uart_config);

    this->device_address = device_address;
}

UART::~UART()
{
    close(this->uart_device);
}

void UART::write_uart(const void *buffer, size_t bytes)
{
    int count;

    count = write(this->uart_device, buffer, bytes);

    if (count < 0)
        throw "Couldn't write on UART";

    return;
}

void UART::read_uart(void *buffer, size_t bytes)
{
    int count;

    count = read(this->uart_device, buffer, bytes);

    if (count < 0)
        throw "Couldn't read from UART";

    return;
}

void UART::send_message(data_interface const *data)
{

    std::cout << "Writing on UART...\n";

    unsigned char *package;
    unsigned short crc;
    int len = 9;

    package = (unsigned char *)malloc(sizeof(unsigned char *) * len + 1);

    package[0] = this->device_address;
    package[1] = data->code;
    package[2] = data->sub_code;

    memcpy((void *)(package + 3), student_id, strlen(student_id));

    if (data->code == WRITE)
    {
        realloc_pointer(package, &len, sizeof(int));

        memcpy((void *)(package + 7), data->data, sizeof(int));
    }

    crc = calcula_CRC(package, len - 2);

    memcpy((void *)(package + (len - 2)), (void *)&crc, 1);

    crc = calcula_CRC(package, len - 1);

    memcpy((void *)(package + (len - 1)), (void *)&crc, 1);

    write_uart((void *)package, len);

    sleep(1);

    std::cout << "Task complete successfuly\n";

    free(package);

    return;
}

void UART::read_message(data_interface *data)
{
    std::cout << "Reading from UART...\n";

    int data_bytes = 0;

    read_uart((void *)&data->address, sizeof(char));

    read_uart((void *)&data->code, sizeof(char));

    read_uart((void *)&data->sub_code, sizeof(char));

    switch (data->sub_code)
    {
    case READ_STATE:
        data_bytes = sizeof(int);
        data->data = malloc(sizeof(int));

        break;
    case READ_TI:
    case READ_TR:
        data_bytes = sizeof(float);
        data->data = malloc(sizeof(float));

        break;
    default:
        throw "Wrong option code";
        break;
    }

    read_uart(data->data, data_bytes);

    read_uart((void *)&data->crc1, 1);
    read_uart((void *)&data->crc2, 1);

    std::cout << "Task complete successfuly\n";

    return;
}