#include "esp_wifi.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include "wifi.h"

//defined in config file to avoid leaking secrets
#define SSID CONFIG_ESP_WIFI_SSID
#define WIFI_PASSWORD CONFIG_ESP_WIFI_PASSWORD 


void on_wifi_disconnect(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data){
    ESP_LOGI("wifi", "Wifi disconnected, trying to reconnect...");
    esp_err_t err = esp_wifi_connect();
    if (err == ESP_ERR_WIFI_NOT_STARTED) {
        return;
    }
    ESP_ERROR_CHECK(err);
}

void wifi_connect(){
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = SSID,
            .password= WIFI_PASSWORD,
        },
    };

    ESP_LOGI("wifi","Connecting to %s...", wifi_config.sta.ssid);
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_ERROR_CHECK(esp_wifi_connect());

    //call on_wifi_disconnect() if wifi connection gets lost
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &on_wifi_disconnect, NULL, NULL));
}

