#ifndef PARSER_H
#define PARSER_H

#define MAX_SENSORS 5
#define MAX_ERRORS 10
#define MAX_MSG_SIZE 1000
#define MAX_ERRMSG_SIZE 256 

typedef struct temperatures{
    char romcode[16+1];
    double temperature;
} t_temperatures;

typedef struct parse_output{
    t_temperatures temps[MAX_SENSORS];
    int num_sensors;
    char msg[MAX_MSG_SIZE];
    char errmsg[MAX_ERRORS][MAX_ERRMSG_SIZE];
    int err_count;
} t_parse_output;

int parse(char* wrd, t_parse_output* results);

#endif //PARSER_H
