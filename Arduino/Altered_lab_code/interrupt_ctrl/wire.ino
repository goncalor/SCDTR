#include "utils.h"
#include <Wire.h>

#define BUF_SPLIT_LEN 15

void wire_process_incoming(char *str)
{
    char *lst[BUF_SPLIT_LEN];
    short numwords;
    float fl;
    char itoabuf[10];

    #ifdef DEBUG
    Serial.print("incoming wire str: ");
    Serial.println(str);
    #endif

    numwords = split(str+1, lst, BUF_SPLIT_LEN);

    switch(str[0])
    {
        case 'l':
            fl = atof(lst[0]);
            noInterrupts();
            ctrl_ref = luxfunction(fl);
            ctrl_mapped_ref = map(ctrl_ref, 0, 255, 0, 1023);
            ref_feedfoward = ctrl_mapped_ref * feedforward_gain;
            interrupts();   
            break;

        case 'a':
            // reply to master with the value on the LDR
            itoa(AnalogReadAvg(analogInPin, NUM_SAMPLES), itoabuf, 10);
            Wire.beginTransmission(MASTER_ID);
            Wire.write(itoabuf);
            Wire.endTransmission();
            break;

        default:
            break;
    }
}


void calibrate()
{
    short i;

    for(i=1; i<=3; i++)
    {
        if(i == MASTER_ID)
           continue;
        Wire.beginTransmission(i);
        Wire.write('a');
        Wire.endTransmission();

        delay(1);
    }
}

