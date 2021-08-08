#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "wifi.h"


void app_main(void){
    ESP_ERROR_CHECK(nvs_flash_init());
    wifi_connect();
    vTaskDelay(2000 / portTICK_PERIOD_MS);
}
