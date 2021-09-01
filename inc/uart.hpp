#ifndef UART_HPP

#define UART_HPP

#include <termios.h>

#define READ 0x23
#define READ_TI 0xC1
#define READ_TR 0xC2
#define READ_STATE 0xC3

#define WRITE 0x16
#define WRITE_SIGNAL 0xD1

using namespace std;

struct data_interface
{
    char code, sub_code;
    char address;
    char crc1, crc2;
    void *data;
} typedef data_interface;

class UART
{
private:
    int uart_device;
    char device_address;
    char *student_id;

    void write_uart(const void *buffer, size_t bytes);
    void read_uart(void *buffer, size_t bytes);

public:
    UART(const char *path, int o_flag, int device_address, const char *student_id);
    ~UART();

    void send_message(data_interface const *data);
    void read_message(data_interface *data);
};

#endif