#ifndef UART_HPP

#define UART_HPP

#include <termios.h>

#define READ 0x23
#define RD_INT 0xA1
#define RD_FLT 0xA2
#define RD_CHR 0xA3

#define WRITE 0x16
#define WT_INT 0xB1
#define WT_FLT 0xB2
#define WT_CHR 0xB3

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

    void write_uart(const void *buffer, size_t bytes);
    void read_uart(void *buffer, size_t bytes);

public:
    UART(char path[256], int o_flag, struct termios uart_config, int device_address);
    ~UART();

    void send_message(data_interface const *data);
    void read_message(data_interface *data);
};

#endif