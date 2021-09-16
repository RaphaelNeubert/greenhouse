#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

int match(char* input, int* pos, char c){
	if (input[*pos] == c){
		(*pos)++;
		return 1;
	}
	else return 0;
}

int matchwrd(char* input, int* pos, char* wrd){
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
int matchspace(char* input, int* pos){
    while (match(input, pos, ' ') || match(input, pos, '\n'));
    return 1;
}

int get_rom_code(char* input, int* pos, t_parse_output* results, int n){
    int p=*pos;
    for (int i=0; i<16; i++){
        if ((input[p] >= 'a' && input[p] <='f') || (input[p] >= '0' && input[p] <= '9')) p++;
        else return 0;
    }
    (*pos) = p;
    strncpy(results->temps[n].romcode, input+*pos-16, 16);
    return 1;
}
int get_value(char* input, int* pos, t_parse_output* results, int n){
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
    results->temps[n].temperature = atof(strvalue);
    results->num_sensors++;
    return 1;
}


int data(char* input, int* pos, t_parse_output* results, int n){
    if (matchwrd(input, pos, "sensor")){
        return matchspace(input, pos) && get_rom_code(input, pos, results, n) && matchspace(input, pos) &&
               match(input, pos, ':') && matchspace(input, pos) && get_value(input, pos, results, n);
    }
    //else if (matchwrd(input, pos, "<other keyword>"))
    else return 0;
}
int next(char* input, int* pos, t_parse_output* results, int n){
    if (match(input, pos, ',')){
        //increase index of result array
        n++;
        return matchspace(input, pos) && data(input, pos, results, n);
    }
    //more data is optional
    return 1;
}

int checktype(char* input, int* pos, t_parse_output* results, int n){
    //data
    if (match(input, pos, '{')){
        return matchspace(input, pos) && data(input, pos, results, n) && matchspace(input, pos) &&
               next(input, pos, results, n) && matchspace(input, pos) && match(input, pos, '}') &&
               checktype(input, pos, results, n);
    }
    //output messages
    else if (input[*pos] != '#'){
        int i=strlen(results->msg);
        while (1){
            if (input[*pos] == '#') return 1;
            else if (input[*pos] == '{') return checktype(input, pos, results, n);
            else {
                results->msg[i] = input[*pos];
                i++;
                (*pos)++;
            }
        }
    }
    else return 1;
}

int parse(char* wrd, t_parse_output* results){
	int pos=0;
    int n=0; //result array index

	int len=strlen(wrd);
	char input[len+2];
	strcpy(input,wrd);
	input[len]='#';
	input[len+1]='\0';
	return matchspace(input, &pos) && checktype(input, &pos, results, n) && match(input, &pos, '#');
}

int main(){
    t_parse_output results = {0};
	printf("%s\n",parse("test   {    sensor    1abc129041fe2838   :   -253429834117220.52914147   ,  sensor abc1290238528375: 12.384  } test", &results)?"accepted":"doesn't match syntax");
    for (int i=0; i<results.num_sensors; i++){
        puts("---------------------");
        printf("romcode: %s, temperature: %f\n", results.temps[i].romcode, results.temps[i].temperature);
    }
    printf("Message: %s\n", results.msg);
    return 0;
}
