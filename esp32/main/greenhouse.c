#include "string.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "wifi.h"
#include "tcp-client.h"


void app_main(void){
    ESP_ERROR_CHECK(nvs_flash_init());
    wifi_connect();
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    for (int i=0;;i++){
        char wrd[30];
        sprintf(wrd, "Hello World: %d", i);
        while (send_data(wrd)!=0) vTaskDelay(1000 / portTICK_PERIOD_MS);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}
