#ifndef TASK_TWO_H
#define TASK_TWO_H

// Global variables
extern char ReceiveBuffer[512];
extern int ReceiveBufferLen;

// Struct declarations
struct Device_id {
    char dev_type[16];
    char serial[16];
    char manuf_date[16];
    char firmw_ver[16];
};

struct Device_params {
    double temp;
    double supp_volt;
    int alarms;
    int gain;
};

// Global instances of structs
extern struct Device_id dev_id;
extern struct Device_params dev_params;

// Function declarations
void ReadReceiveBuffer(char *RecBuff); //ReadReceiveBuffer(ReceiveBuffer) should be called if we stopped receiving messages

#endif // TASK_TWO_H