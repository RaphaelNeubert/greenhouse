#include "esp_log.h"
#include "sdkconfig.h"
#include "lwip/sockets.h"
#include "lwip/err.h"
#include "tcp-client.h"

//defined in config file to avoid leaking secrets
#define TCP_SERVER_IP CONFIG_TCP_SERVER_IP
#define TCP_SERVER_PORT CONFIG_TCP_SERVER_PORT 

static int establish_connection(){
    struct sockaddr_in dest_addr;
    dest_addr.sin_addr.s_addr = inet_addr(TCP_SERVER_IP);
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(TCP_SERVER_PORT);
   

    //create socket
    int s = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if (s < 0) {
        ESP_LOGE("TCP", "Unable to create socket: errno %s", strerror(errno));
        return -1;
    }
    ESP_LOGI("TCP", "Socket created, connecting to %s:%d", TCP_SERVER_IP, TCP_SERVER_PORT);

    //connect to server
    if (connect(s, (struct sockaddr*)&dest_addr, sizeof(struct sockaddr_in)) != 0) {
        ESP_LOGE("TCP", "Socket unable to connect: %s", strerror(errno));
        shutdown(s, 0);
        close(s);
        return -2;
    }
    ESP_LOGI("TCP", "Socket successfully connected");
    return s;
}

int send_data(char* msg){
    int s = establish_connection();
    if (s<0){
        ESP_LOGE("TCP", "Failed to establish the connection. return value: %d", s);
        return -1;
    }
    //send message
    if (send(s, msg, strlen(msg), 0) < 0) {
        ESP_LOGE("TCP", "Error occurred during sending:%s", strerror(errno));
        shutdown(s, 0);
        close(s);
        return -2;
    }

    ESP_LOGI("TCP", "Shutting down socket...");
    shutdown(s, 0);
    close(s);
    return 0;
}
