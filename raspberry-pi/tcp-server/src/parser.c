#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"

static int match(char* input, int* pos, char c){
	if (input[*pos] == c){
		(*pos)++;
		return 1;
	}
	else return 0;
}
 
static int matchwrd(char* input, int* pos, char* wrd){
    int n = strlen(wrd);
    int p=*pos;
    for (int i=0; i<n; i++){
        if (input[p] == wrd[i]) p++;
        else return 0;
    }
    (*pos) = p;
    return 1;
}
//ignore spaces or newline
static int matchspace(char* input, int* pos){
    while (match(input, pos, ' ') || match(input, pos, '\n'));
    return 1;
}

static int get_rom_code(char* input, int* pos, t_parse_output* results){
    int p=*pos;
    for (int i=0; i<16; i++){
        if ((input[p] >= 'a' && input[p] <='f') || (input[p] >= '0' && input[p] <= '9')) p++;
        else return 0;
    }
    (*pos) = p;
    strncpy(results->temps[results->num_sensors].romcode, input+*pos-16, 16);
    results->temps[results->num_sensors].romcode[16] = '\0';
    return 1;
}

static int get_value(char* input, int* pos, t_parse_output* results){
    int i=0;
    int p=*pos;
    short int dot = 0;
    short int minus= 0;
    char strvalue[25];

    if (input[p] == '-'){
        minus++;
        p++;
        i++;
    }
    for (; i<25; i++){
        if (input[p] >= '0' && input[p] <= '9');
        else if (input[p] == '.' && dot == 0) dot++;
        else if (input[p] == '.' && dot != 0) return 0;
        else break;
        p++;
    }
    (*pos)=p;
    strncpy(strvalue, input+*pos-i, i);
    results->temps[results->num_sensors].temperature = atof(strvalue);
    results->num_sensors++;
    return 1;
}
static int get_errmsg(char* input, int* pos, t_parse_output* results){
    int i=0;
    while (1){
        if (input[*pos] == '#') return 0;
        else if (input[*pos] == '}'){
            results->err_count++;
            return 1;
        }
        else if (input[*pos] == ','){
            results->err_count++;
            return 1;
        }
        else{
            results->errmsg[results->err_count][i] = input[*pos];
            results->errmsg[results->err_count][i+1] = '\0';
            (*pos)++;
            i++;
        }
    }
}

static int data(char* input, int* pos, t_parse_output* results){
    if (matchwrd(input, pos, "sensor")){
        return matchspace(input, pos) && get_rom_code(input, pos, results) && matchspace(input, pos) &&
               match(input, pos, ':') && matchspace(input, pos) && get_value(input, pos, results);
    }
    else if (matchwrd(input, pos, "error")){
        return matchspace(input, pos) && match(input, pos, ':') && matchspace(input, pos) &&
               get_errmsg(input, pos, results) && matchspace(input, pos);
    }
    else return 0;
}
static int next(char* input, int* pos, t_parse_output* results){
    if (match(input, pos, ',')){
        return matchspace(input, pos) && data(input, pos, results);
    }
    //more data is optional
    return 1;
}

static int checktype(char* input, int* pos, t_parse_output* results){
    //data
    if (match(input, pos, '{')){
        return matchspace(input, pos) && data(input, pos, results) && matchspace(input, pos) &&
               next(input, pos, results) && matchspace(input, pos) && match(input, pos, '}') &&
               checktype(input, pos, results);
    }
    //output messages
    else if (input[*pos] != '#'){
        int i=strlen(results->msg);
        while (1){
            if (input[*pos] == '#') return 1;
            else if (input[*pos] == '{') return checktype(input, pos, results);
            else {
                results->msg[i] = input[*pos];
                results->msg[i+1] = '\0';
                i++;
                (*pos)++;
            }
        }
    }
    else return 1;
}

int parse(char* wrd, t_parse_output* results){
	int pos=0;

	int len=strlen(wrd);
	char input[len+2];
	strcpy(input,wrd);
	input[len]='#';
	input[len+1]='\0';
	return matchspace(input, &pos) && checktype(input, &pos, results) && match(input, &pos, '#');
}
/*
int main(){
    t_parse_output results = {0};
	printf("%s\n",parse("test   {    sensor    1abc129041fe2838   :   -253429834117220.52914147   ,  sensor abc1290238528375: 12.384  } test", &results)?"accepted":"doesn't match syntax");
    for (int i=0; i<results.num_sensors; i++){
        puts("---------------------");
        printf("romcode: %s, temperature: %f\n", results.temps[i].romcode, results.temps[i].temperature);
    }
    printf("Message: %s\n", results.msg);
    t_parse_output results2 = {0};
	printf("%s\n",parse("test   {    error : invalid sensor value: sensor 1abc129041fe2838 value 80.00000  ,  error: invalid sensor value: sensor abc1290238528375 value -30.00}", &results2)?"accepted":"doesn't match syntax");
    for (int i=0; i<results2.err_count; i++){
        printf("%s\n", results2.errmsg[i]);
    }
    printf("Message: %s\n", results2.msg);
    return 0;
}
*/
