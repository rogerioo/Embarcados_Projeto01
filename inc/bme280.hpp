#ifndef __BME280__
#define __BME280__

struct sensor_data
{
    float temperature;
    float pressure;
    float humidity;
} typedef sensor_data;

class BME280
{
private:
    int bme280Init(int iChannel, int iAddr);
    int bme280ReadValues(int *T, int *P, int *H);

    sensor_data response;

public:
    BME280(int channel, int address);
    ~BME280();

    sensor_data *get_data();
};

#endif