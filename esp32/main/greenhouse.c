#include "string.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"

#include "wifi.h"
#include "tcp-client.h"
#include "sensor.h"

#define SAMPLE_PERIOD 60
#define RETRIES 10
//measured values outside of this intervall will be considered invalid
#define MAX_TEMP 30.0
#define MIN_TEMP (-20.0)


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
            //check if values are valid and send errmsg if not
            if (readings[i].value > MAX_TEMP || readings[i].value < MIN_TEMP){
                sprintf(submsg+strlen(submsg),"%s error: invalid sensor value: sensor %s value %f",
                        (i!=0)?",":"", readings[i].romcode, readings[i].value);
            }
            else{
                sprintf(submsg+strlen(submsg),"%s sensor %s: %f", (i!=0)?",":"", readings[i].romcode, readings[i].value);
            }
        }
        sprintf(msg, "{%s}", submsg);
        //send message
        for (int i=0; send_data(msg)!=0 && i<RETRIES; i++){
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
        vTaskDelay(SAMPLE_PERIOD*1000 / portTICK_PERIOD_MS);
    }
}
