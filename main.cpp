#include <iostream>
#include "uart.hpp"
#include <fcntl.h>
#include <string.h>

using namespace std;

int main(int argc, char const *argv[])
{
    char device_path[] = "/dev/serial0";
    int o_flag = O_RDWR | O_NOCTTY | O_NDELAY;
    struct termios uart_config;

    uart_config.c_cflag = B9600 | CS8 | CLOCAL | CREAD;
    uart_config.c_iflag = IGNPAR;
    uart_config.c_oflag = 0;
    uart_config.c_lflag = 0;

    UART *uart = new UART(device_path, o_flag, uart_config, 0x01);

    system("clear");

    while (true)
    {
        data_interface request, response;

        int it_data;
        float ft_data;
        string st_data;

        int option;

        cout << "************** UART MODBUS V1.0 **************" << endl;
        cout << "What do you wanna do?\n"
             << endl;
        cout << "(1) Ask an Interger" << endl;
        cout << "(2) Ask a Float" << endl;
        cout << "(3) Ask a Char Array" << endl;
        cout << "(4) Send as Interger" << endl;
        cout << "(5) Send a Float" << endl;
        cout << "(6) Send a Char Array"
             << endl;
        cout << "\nAny other number to leave\n"
             << endl;

        cin >> option;

        switch (option)
        {
        case 1:
            cout << "\nAsking Interger to ESP trough UART..." << endl;

            request.code = READ;
            request.sub_code = RD_INT;

            uart->send_message(&request);
            uart->read_message(&response);

            cout << "\nThe response is:" << endl;
            cout << (*((int *)response.data)) << endl;

            free(response.data);

            break;
        case 2:
            cout << "\nAsking Float to ESP trough UART..." << endl;

            request.code = READ;
            request.sub_code = RD_FLT;

            uart->send_message(&request);
            uart->read_message(&response);

            cout << "\nThe response is:" << endl;
            cout << (*((float *)response.data)) << endl;

            free(response.data);

            break;
        case 3:
            cout << "\nAsking Char Array to ESP trough UART..." << endl;

            request.code = READ;
            request.sub_code = RD_CHR;

            uart->send_message(&request);
            uart->read_message(&response);

            cout << "\nThe response is:" << endl;
            cout << ((char *)response.data) << endl;

            free(response.data);

            break;
        case 4:
            cout << "\nSending Interger to ESP trough UART...\n"
                 << endl;

            request.code = WRITE;
            request.sub_code = WT_INT;

            request.data = (int *)malloc(sizeof(int));

            cout << "What the Interger do you want to send?\n"
                 << endl;

            cin >> it_data;

            cout << endl;

            *((int *)request.data) = it_data;

            uart->send_message(&request);
            uart->read_message(&response);

            cout << "\nThe response is:" << endl;
            cout << (*((int *)response.data)) << endl;

            free(request.data);
            free(response.data);

            break;
        case 5:
            cout << "\nSending Float to ESP trough UART...\n"
                 << endl;

            request.code = WRITE;
            request.sub_code = WT_FLT;

            request.data = (float *)malloc(sizeof(float));

            cout << "What the Float do you want to send?\n"
                 << endl;

            cin >> ft_data;
            cout << endl;

            *((float *)request.data) = ft_data;

            uart->send_message(&request);
            uart->read_message(&response);

            cout << "\nThe response is:" << endl;
            cout << (*((float *)response.data)) << endl;

            free(request.data);
            free(response.data);

            break;
        case 6:
            cout << "\nSending Char Array to ESP trough UART...\n"
                 << endl;

            request.code = WRITE;
            request.sub_code = WT_CHR;

            while (1)
            {
                cout << "What the String do you want to send? (Max 256 char.)\n"
                     << endl;

                cin >> st_data;
                cout << endl;

                if (st_data.length() > 256)
                {
                    cout << "Maximum exceeded!" << endl;
                    continue;
                }

                break;
            }

            request.data = (char *)malloc(sizeof(char) * st_data.length() + 1);
            strcpy(((char *)request.data), st_data.c_str());

            uart->send_message(&request);
            uart->read_message(&response);

            cout << "\nThe response is:" << endl;
            cout << ((char *)response.data) << endl;

            free(request.data);
            free(response.data);

            break;

        default:
            char leave;

            cout << "\nDou you really wanna leave? (Y/N)\n"
                 << endl;

            cin >> leave;

            if (leave == 'Y' || leave == 'y')
                return 0;

            break;
        }

        cout << "\nPress ENTER to continue..\n"
             << endl;
        getchar();
        getchar();

        system("clear");
    }
}
