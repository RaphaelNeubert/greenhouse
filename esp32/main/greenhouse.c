#include "string.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"

#include "wifi.h"
#include "tcp-client.h"
#include "sensor.h"

void app_main(void){
    ESP_ERROR_CHECK(nvs_flash_init());
    wifi_connect();
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    t_sensor_info* sensors = init_sensors();
    while (1){
        char msg[250]="";
        float readings[sensors->num_devices];
        measure_temp(sensors, readings);
        for (int i=0; i<sensors->num_devices; i++){
            //append string
            sprintf(msg+strlen(msg),"sensor: %d, value: %.1f\n", i, readings[i]);
        }
        while (send_data(msg)!=0) vTaskDelay(1000 / portTICK_PERIOD_MS);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}
