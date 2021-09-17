#ifndef PARSER_H
#define PARSER_H

#define MAX_SENSORS 5
#define MAX_MSG_SIZE 1000

typedef struct temperatures{
    char romcode[16+1];
    double temperature;
} t_temperatures;

typedef struct parse_output{
    t_temperatures temps[MAX_SENSORS];
    int num_sensors;
    char msg[MAX_MSG_SIZE];
} t_parse_output;

int parse(char* wrd, t_parse_output* results);

#endif //PARSER_H
