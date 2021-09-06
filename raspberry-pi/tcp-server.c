#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

#define RECV_BUF_SIZE 500
#define PORT 8080
  
int main(){
    //loop to make it retry in case of failure 
    while (1){
        int server_socket, client_socket, n;
        struct sockaddr_in server_sockaddr;
        char buf[RECV_BUF_SIZE];

        bzero(&server_sockaddr, sizeof(server_sockaddr));

        //server socket configurations
        server_sockaddr.sin_family = AF_INET;
        server_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
        server_sockaddr.sin_port = htons(PORT);
        server_socket = socket(AF_INET, SOCK_STREAM, 0);

        //socket creation
        if (server_socket == -1) {
            printf("Socket creation failed. errno: %s\n", strerror(errno));
            sleep(5);
            continue;
        }
        printf("Socket successfully created.\n");
        
        //socket bind
        if ((bind(server_socket, (struct sockaddr*)&server_sockaddr, sizeof(server_sockaddr))) != 0) {
            printf("Socket bind failed. errno: %s\n", strerror(errno));
            close(server_socket);
            sleep(5);
            continue;
        }
        printf("Socket successfully binded.\n");
        
        //listen for connections on socket
        if ((listen(server_socket, 5)) != 0) {
            printf("Listen failed. errno: %s\n", strerror(errno));
            close(server_socket);
            sleep(5);
            continue;
        }
        printf("Server listening..\n");
      
        while (1){
            struct sockaddr_in client_sockaddr;
            socklen_t client_sockaddr_len;

            //accept connection on socket
            client_socket = accept(server_socket, (struct sockaddr*) &client_sockaddr, &client_sockaddr_len);
                if (client_socket < 0){
                    printf("server accept failed... errno: %s\n", strerror(errno));
                    sleep(2);
                    continue;
                }
            memset(buf, 0, RECV_BUF_SIZE);

            //read message from client socket until eof
            while ((n=read(client_socket, buf, RECV_BUF_SIZE-1)) > 0);
                if (n >= 0){ //success
                    printf("%s\n", buf);
                }
                else {
                    printf("socket read failed. errno: %s\n", strerror(errno));
                }
            close(client_socket);
        }
        close(server_socket);
    }
    return 0;
}
