#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SENSORS 5

typedef struct value{
    char romcode[16+1];
    float temperature;
} t_value;

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

int get_rom_code(char* input, int* pos, t_value* results, int n){
    int p=*pos;
    for (int i=0; i<16; i++){
        if ((input[p] >= 'a' && input[p] <='f') || (input[p] >= '0' && input[p] <= '9')) p++;
        else return 0;
    }
    (*pos) = p;
    strncpy(results[n].romcode, input+*pos-16, 16);
    return 1;
}
int get_value(char* input, int* pos, t_value* results, int n){
    int i=0;
    int p=*pos;
    short int dot = 0;
    short int minus= 0;
    char strvalue[9];

    if (input[p] == '-'){
        minus++;
        p++;
        i++;
    }
    for (; i<8; i++){
        if (input[p] >= '0' && input[p] <= '9');
        else if (input[p] == '.' && dot == 0) dot++;
        else if (input[p] == '.' && dot != 0) return 0;
        else break;
        p++;
    }
    (*pos)=p;
    strncpy(strvalue, input+*pos-i, i);
    results[n].temperature = atof(strvalue);
    return 1;
}


int data(char* input, int* pos, t_value* results, int n){
    if (matchwrd(input, pos, "sensor")){
        return matchspace(input, pos) && get_rom_code(input, pos, results, n) && matchspace(input, pos) &&
               match(input, pos, ':') && matchspace(input, pos) && get_value(input, pos, results, n);
    }
    //else if (matchwrd(input, pos, "<other keyword>"))
    else return 0;
}
int next(char* input, int* pos, t_value* results, int n){
    if (match(input, pos, ',')){
        //increase index of result array
        n++;
        return matchspace(input, pos) && data(input, pos, results, n);
    }
    //more data is optional
    return 1;
}

int checktype(char* input, int* pos, t_value* results, int n){
    //data
    if (match(input, pos, '{')){
        return matchspace(input, pos) && data(input, pos, results, n) && matchspace(input, pos) &&
               next(input, pos, results, n) && matchspace(input, pos) && match(input, pos, '}');
    }
    //message
    else if (input[*pos] != '\0'){
        printf("Message: %s\n", input[*pos]);
        return 1;
    }
    else return 0;
}

int parse(char* wrd, t_value* results){
	int pos=0;
    int n=0;

	int len=strlen(wrd);
	char input[len+2];
	strcpy(input,wrd);
	input[len]='#';
	input[len+1]='\0';
	return matchspace(input, &pos) && checktype(input, &pos, results, n) && match(input, &pos, '#');
}

int main(){
    t_value results[MAX_SENSORS] = {0};
	printf("%s\n",parse("{    sensor    1abc129041fe2838   :   -20.529   ,  sensor abc1290238528375: 12.384}", results)?"accepted":"doesn't match syntax");
    for (int i=0; strlen(results[i].romcode) > 0; i++){
        puts("---------------------");
        printf("romcode: %s, temperature: %0.3f\n", results[i].romcode, results[i].temperature);
    }
    return 0;
}
