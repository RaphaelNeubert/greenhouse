#include "string.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"

#include "wifi.h"
#include "tcp-client.h"
#include "sensor.h"

#define SAMPLE_PERIOD 30
#define RETRIES 10

void app_main(void){
    ESP_ERROR_CHECK(nvs_flash_init());
    wifi_connect();
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    t_sensor_info* sensors = init_sensors();
    while (1){
        char submsg[256]="";
        char msg[512]="";
        t_sensor_results readings[sensors->num_devices];
        measure_temp(sensors, readings);
        //create string following the syntax that will be parsed
        for (int i=0; i<sensors->num_devices; i++){
            //append string
            sprintf(submsg+strlen(submsg),"%s sensor %s: %f", (i!=0)?",":"", readings[i].romcode, readings[i].value);
        }
        sprintf(msg, "test {%s}", submsg);
        //send message
        for (int i=0; send_data(msg)!=0 && i<RETRIES; i++){
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }        vTaskDelay(SAMPLE_PERIOD*1000 / portTICK_PERIOD_MS);
    }
}
