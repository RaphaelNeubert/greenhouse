#ifndef SENSORS_H
#define SENSORS_H

#include "owb.h"
#include "owb_rmt.h"
#include "ds18b20.h"

#define GPIO_DS18B20 4
#define MAX_DEVICES 2
#define DS18B20_RESOLUTION   (DS18B20_RESOLUTION_12_BIT)

typedef struct sensor_info{
    OneWireBus* owb;
    DS18B20_Info* devices[MAX_DEVICES];
    int num_devices;
}t_sensor_info;

t_sensor_info init_sensors();
int measure_temp(t_sensor_info sensors, float* readings);


#endif
