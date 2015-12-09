#include "utils.h"
#include <Wire.h>

#define BUF_SPLIT_LEN 15

void wire_process_incoming(char *str)
{
    char *lst[BUF_SPLIT_LEN];
    short numwords;
    float fl;
    int in;
    char itoabuf[10];

    wire_data_available = false;

    #ifdef DEBUG
    Serial.print("incoming wire str: ");
    Serial.println(str);
    #endif

    numwords = split(str+1, lst, BUF_SPLIT_LEN);

    switch(str[0])
    {
        case 'l':
            // set reference in lux
            // 'l lux'
            fl = atof(lst[0]);
            noInterrupts();
            ctrl_ref = luxfunction(fl);
            ctrl_mapped_ref = map(ctrl_ref, 0, 255, 0, 1023);
            ref_feedfoward = ctrl_mapped_ref * feedforward_gain;
            interrupts();   
            break;

        case 'a':
            // reply to master with the value on the LDR
            // reply to send 'b my_address ldr'
            Wire.beginTransmission(MASTER_ID);
            Wire.write("b ");
            itoa(wire_my_address, itoabuf, 10);
            Wire.write(itoabuf);    // send my address
            Wire.write(' ');
            itoa(AnalogReadAvg(analogInPin, NUM_SAMPLES), itoabuf, 10);
            Wire.write(itoabuf);    // send my LDR reading
            Wire.endTransmission();
            break;

        case 'b':
            // reserved
            break;

        case 'd':
            disable_controller();
            break;

        case 'e':
            enable_controller();
            break;

        case 'p':
            // set the reference in PWM value
            // 'p pwm'
            in = atoi(lst[0]);
            noInterrupts();
            ctrl_mapped_ref = map(in, 0, 255, 0, 1023);
            ref_feedfoward = ctrl_mapped_ref * feedforward_gain;
            interrupts();   
            break;

        case 'q':
            // set the LED to a certain PWM value 
            // 'q pwm'
            analogWrite(analogOutPin, atoi(lst[0]));
            break;

        default:
            break;
    }
}

// TODO: turn off controller while calibrating
// TODO: distribute O
// TODO: distribute E

int O_vals[3];
int E_vals[3][3];

void calibrate()
{
    //get_O();
    get_E();
}


void disable_all_controllers()
{
    disable_controller();
    Wire.beginTransmission(0);
    Wire.write("d");
    Wire.endTransmission();
}


void enable_all_controllers()
{
    enable_controller();
    Wire.beginTransmission(0);
    Wire.write("e");
    Wire.endTransmission();
}


void get_O()
{
    short i;
    char *lst[BUF_SPLIT_LEN];
    short dev_id;
    short numwords;

    disable_all_controllers();

    // turn the lights off
    // the others
    Wire.beginTransmission(0);
    Wire.write("q 0");
    Wire.endTransmission();
    // my own
    analogWrite(analogOutPin, 0);

    delay(2000);    // wait for others to turn off and stabilize

    for(i=1; i<=3; i++)
    {
        if(i == MASTER_ID)
            continue;
        Wire.beginTransmission(i);
        Wire.write("a");
        Wire.endTransmission();

        while(!wire_data_available)
            ;   // wait for data
        wire_data_available = false;

        numwords = split(wire_buf, lst, BUF_SPLIT_LEN);
        if(numwords==3 && wire_buf[0]=='b')
        {
            dev_id = atoi(lst[1]);
            O_vals[dev_id-1] = atoi(lst[2]);
        }
    }

    O_vals[MASTER_ID-1] = AnalogReadAvg(analogInPin, NUM_SAMPLES);

    #ifdef DEBUG
    Serial.print("O_vals: ");
    for(i=0; i<3; i++)
    {
        Serial.print(O_vals[i]);
        Serial.print(" ");
    }
    Serial.println("");
    #endif

    enable_all_controllers();
}


void get_E()
{
    short i;
    char *lst[BUF_SPLIT_LEN];
    short dev_id;
    short numwords;

    disable_all_controllers();
    delay(1000);

    // turn the lights off
    // the others
    Wire.beginTransmission(0);
    Wire.write("q 0");
    Wire.endTransmission();
    // my own
    analogWrite(analogOutPin, 0);

    delay(2000);    // wait for others to turn off and stabilize

    Wire.beginTransmission(0);
    Wire.write("q 255");
    Wire.endTransmission();

    delay(2000);    // wait for others to turn off and stabilize


    enable_all_controllers();
}
