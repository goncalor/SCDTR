#include "utils.h"
#include <Wire.h>

#define BUF_SPLIT_LEN 15

void wire_process_incoming(char *str)
{
    char *lst[BUF_SPLIT_LEN];
    short numwords;
    float fl;

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

        default:
            break;
    }

}

