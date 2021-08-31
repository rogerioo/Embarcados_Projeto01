#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include "uart.hpp"
#include "crc.hpp"

using namespace std;

void realloc_pointer(unsigned char *package, int *len, size_t bytes)
{
    unsigned char *tmp = package;

    *len += bytes;

    tmp = (unsigned char *)realloc(tmp, *len);
    package = tmp;

    return;
}

UART::UART(char path[256], int o_flag, struct termios uart_config, int device_address)
{
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
    int len = 5;

    package = (unsigned char *)malloc(sizeof(char) * len);

    package[0] = this->device_address;
    package[1] = data->code;
    package[2] = data->sub_code;

    if (data->code == WRITE)
    {
        switch (data->sub_code)
        {
        case WT_INT:
        {
            realloc_pointer(package, &len, sizeof(int));

            memcpy((void *)(package + 3), data->data, sizeof(int));

            write_uart(&data->data, sizeof(int));

            break;
        }
        case WT_FLT:
        {
            realloc_pointer(package, &len, sizeof(float));

            memcpy((void *)(package + 3), data->data, sizeof(float));

            write_uart(&data->data, sizeof(float));
            break;
        }
        case WT_CHR:
        {
            size_t length = sizeof(data->data);

            realloc_pointer(package, &len, 1 + length);

            memcpy((void *)(package + 3), (void *)&length, sizeof(char));
            memcpy((void *)(package + 4), data->data, sizeof(char) * length);

            break;
        }

        default:
            throw "Wrong option code";
            break;
        }
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
    case RD_INT:
    case WT_INT:
        data_bytes = sizeof(int);
        data->data = malloc(sizeof(int));

        break;
    case RD_FLT:
    case WT_FLT:
        data_bytes = sizeof(float);
        data->data = malloc(sizeof(float));

        break;
    case RD_CHR:
    case WT_CHR:
        char string_size;

        read_uart((void *)&string_size, 1);

        data_bytes = (int)string_size;

        data->data = malloc(sizeof(char) * data_bytes + 1);

        break;

    default:
        throw "Wrong option code";
        break;
    }

    read_uart(data->data, data_bytes);

    if (data->sub_code == RD_CHR || data->sub_code == WT_CHR)
        read_uart((void *)&data->crc1, 1);

    read_uart((void *)&data->crc1, 1);
    read_uart((void *)&data->crc2, 1);

    std::cout << "Task complete successfuly\n";

    return;
}