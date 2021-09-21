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
#include <time.h>
#include <sqlite3.h>
#include "parser.h"

#define RECV_BUF_SIZE 500
#define PORT 8080

int logger(char type, const char* msg, const char* msg2){
	FILE* fp;
	time_t now;
	char timestr[30];

	time(&now);
	strcpy(timestr,ctime(&now));
	timestr[strlen(timestr)-1]='\0';

	fp=fopen("log","a");
	if (fp == NULL) return -1;

    if (type == 'I'){
        fprintf(fp, "%s [Info] %s %s\n", timestr, msg, msg2);
    }
    else if (type == 'E'){
        fprintf(fp, "%s [Error] %s %s, errno: %s\n", timestr, msg, msg2, strerror(errno));
    }

	fclose(fp);
    return 0;
}


int msg_recieved(char* msg){
    sqlite3* db;
    char sqlstatement[256];
    char *errmsg = NULL;
    int sqlcheck;
    t_parse_output parse_output;

    parse_output.num_sensors = 0;
    parse_output.msg[0] = '\0';
    parse_output.err_count = 0;

    //parse recieved message
    if (parse(msg, &parse_output)){ ;
        //log error messages recieved via TCP 
        for (int i=0; i<parse_output.err_count; i++){
            logger('E', "[ESP] ", parse_output.errmsg[i]);
        }
        sqlcheck = sqlite3_open("../tempdata.db", &db);
        if (sqlcheck){
            logger('E',"Can't open database. sqlite errmsg: ", sqlite3_errmsg(db));
            return -1;
        }
        printf("num sensors: %d\n", parse_output.num_sensors);
        //store recieved values
        for (int i=0; i<parse_output.num_sensors; i++){
            //create sql statement
            sprintf(sqlstatement, "INSERT INTO temperature (timestamp, romcode, value) \n VALUES (%d, '%s', %f);",
                    (unsigned)time(NULL), parse_output.temps[i].romcode, parse_output.temps[i].temperature);
            puts(sqlstatement);
            //execute sql statement
            sqlcheck = sqlite3_exec(db, sqlstatement, NULL, 0, &errmsg);
            if (sqlcheck != SQLITE_OK){
                logger('E',"Error executing SQL statement:", errmsg);
                return -1;
            }
            else puts("success");
        }
        //log optional parser message
        if (strlen(parse_output.msg) > 0) logger('I', "Parser Message:", parse_output.msg);
        
        sqlite3_close(db);
        return 0;
    }
    else {
        logger('E',"Parsing failed, check syntax.", "");
        return -1;
    }
}
  
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
            logger('E',"Socket creation failed", "");
            sleep(5);
            continue;
        }
        logger('I',"Socket successfully created", "");
        
        //socket bind
        if ((bind(server_socket, (struct sockaddr*)&server_sockaddr, sizeof(server_sockaddr))) != 0) {
            logger('E',"Socket bind failed", "");
            close(server_socket);
            sleep(5);
            continue;
        }
        logger('I',"Socket successfully bound", "");
        
        //listen for connections on socket
        if ((listen(server_socket, 5)) != 0) {
            logger('E',"Listen failed", "");
            close(server_socket);
            sleep(5);
            continue;
        }
        logger('I',"Server is listening", "");
      
        while (1){
            struct sockaddr_in client_sockaddr;
            socklen_t client_sockaddr_len;

            //accept connection on socket
            client_socket = accept(server_socket, (struct sockaddr*) &client_sockaddr, &client_sockaddr_len);
                if (client_socket < 0){
                    logger('E',"Server accept failed", "");
                    sleep(2);
                    continue;
                }
            memset(buf, 0, RECV_BUF_SIZE);

            //read message from client socket until eof
            while ((n=read(client_socket, buf, RECV_BUF_SIZE-1)) > 0);
            if (n >= 0){ //success
                printf("%s\n", buf);
                msg_recieved(buf);
            }
            else {
                logger('E',"Socket read failed", "");
            }
            close(client_socket);
        }
        close(server_socket);
    }
    return 0;
}
