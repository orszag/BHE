#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

char resp[256];

struct Device_id{
    char dev_type[16];
    char serial[16];
    char manuf_date[16];
    char firmw_ver[16];
};

struct Device_params{
    double temp;
    double supp_volt;
    int alarms;
    int gain;
};
struct Device_id dev_id = {"BRTK12", "34", "2022-11-23", "v2.4"};
struct Device_params dev_params = {32.4, 11.7, 0x17, 0x05};

char* EncodeHEX(int value, char *str){
    sprintf(str, "%X", value);
    if(strlen(str) == 1){
        str[1] = str[0];
        str[2] = '\0';
        str[0] = '0';
    }
    return str;
}

int DecodeHEX(const char *str){
    for(int i=0;str[i]!='\0';i++){
        if(islower(str[i])){
            return -1;
        }
    }
    return (int)strtol(str, NULL, 16);
}

char* EncodeFLOAT(double value, char *str){
    int i = 0;
    sprintf(str, "%lf", value);
    while(!(str[i] != '0' && str[i+1] == '\0')){ //this loop takes away the tailing zeros
        if(str[i] == '0' && str[i+1] == '\0'){
            str[i] = '\0';
            i = -1;
        }
        i++;
    }
    return str;   
}

double DecodeFLOAT(char *str){
    char int_part[16];
    double divider = 10;
    int i = 0;
    double value = 0; //i'm using double variable because the float var doesn't have enough precision
    bool d_digit = false;
    while (str[i] != '\0') {
        if(str[i] == '.'){
            d_digit = true;
            strncpy(int_part, str, i);
            int_part[i] = '\0';
            value = (float)strtol(int_part, NULL, 10);
        }else if(d_digit){
            value += ((double)(str[i] - '0')) / divider;
            divider *= 10;
        }
        i++;
    }
    return value;
}

char* ProcessMSG(const char *msg, char *response){
    char com_code[5];
    int msg_length = (int)strlen(msg);
    char str_buff[16];
    int int_buff = -1;
    char param_list[248];

    if(msg[0] != '*' || msg[5] != ':'){
        strcpy(response, "!ERRR:Wrong message format##");
        return response;
    }
    for(int i=0;i<=msg_length;i++){
        if(msg[i] == '\0' && (msg[i-1] != '#' || msg[i-2] != '#')){
            strcpy(response, "!ERRR:Wrong message format##");
            return response;
        }   
    }
    if(msg_length > 256){
        strcpy(response, "!ERRR:Message is too long##");
        return response;
    }

    strncpy(com_code, msg+1, 4);
    com_code[4] = '\0';

    int ind = 0;
    while(msg[ind+6] != '#'){
        param_list[ind] = msg[ind+6];
        ind++;
    }
    param_list[ind] = '\0';

    response[0] = '!';
    if(!strcmp(com_code, "ASKI")){
        if(param_list[0]!='\0'){
            strcpy(response, "!ERRR:Wrong value##");
            return response;
        }
        strcat(response, "ASKI:");
        strcat(response, dev_id.dev_type);
        strcat(response, ",");
        strcat(response, dev_id.serial);
        strcat(response, ",");
        strcat(response, dev_id.manuf_date);
        strcat(response, ",");
        strcat(response, dev_id.firmw_ver);
    }else if(!strcmp(com_code, "ASKA")){
        if(param_list[0]!='\0'){
            strcpy(response, "!ERRR:Wrong value##");
            return response;
        }
        strcat(response, "ASKA:");
        strcat(response, EncodeFLOAT(dev_params.temp, str_buff));
        strcat(response, ",");
        strcat(response, EncodeFLOAT(dev_params.supp_volt, str_buff));
        strcat(response, ",");
        strcat(response, EncodeHEX(dev_params.alarms, str_buff));
        strcat(response, ",");
        strcat(response, EncodeHEX(dev_params.gain, str_buff));
    }else if(!strcmp(com_code, "SETG")){
        strcat(response, "SETG:");
        int_buff = DecodeHEX(param_list);
        if(int_buff >= 0x0 && int_buff <= 0x20){
            strcat(response, EncodeHEX(int_buff, str_buff));
            if(strcmp(str_buff,param_list)){
                strcpy(response, "!ERRR:Wrong value##");
                return response;
            }
            dev_params.gain = int_buff;
        }else{
            strcpy(response, "!ERRR:Wrong value##");
            return response;
        }
    }
    strcat(response, "##");
    return response;
}

int main(int argc, char *argv[]){
    if(argc == 1){
        printf("Message needed!\n");
        return 1;
    }else if(argc > 2){
        printf("Only one message can be processed!\n");
        return 2;
    }
    
    printf("%s\n", ProcessMSG(argv[1], resp));

    return 0;
}