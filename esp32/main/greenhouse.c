#include "string.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"

#include "wifi.h"
#include "tcp-client.h"
#include "sensor.h"

void app_main(void){
    //ESP_ERROR_CHECK(nvs_flash_init());
    //wifi_connect();
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    /*
    for (int i=0;;i++){
        char wrd[30];
        sprintf(wrd, "Hello World: %d", i);
        while (send_data(wrd)!=0) vTaskDelay(1000 / portTICK_PERIOD_MS);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
    */
    t_sensor_info* sensors = init_sensors();
    while (1){
        float readings[sensors->num_devices];
        measure_temp(sensors, readings);
        for (int i=0; i<sensors->num_devices; i++){
            printf("sensor: %d, value: %.1f\n", i, readings[i]);
        }
        puts("-----------------");
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}
