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
            lux_ref = fl;
            ctrl_ref = lux_to_pwm(fl);
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

        case 'f':
            // reply to master with the illuminance in lux
            Wire.beginTransmission(MASTER_ID);
            ftoa(adc_to_lux(AnalogReadAvg(analogInPin, NUM_SAMPLES)), itoabuf);
            Wire.write(itoabuf);
            Wire.endTransmission();
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

// TODO: distribute O
// TODO: distribute E

int O_vals[3];
int E_vals[3][3];

void calibrate()
{
    disable_all_controllers();
    get_O();
    get_E();
    enable_all_controllers();
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


/* Writes all the current readings into 'data'. This function blocks while it
 * does not have all the readings. */
void get_all_readings(int *data, short len)
{
    short i;
    char *lst[BUF_SPLIT_LEN];
    short dev_id;
    short numwords;

    for(i=1; i<=len; i++)
    {
        if(i == MASTER_ID)
        {
            // do my own reading
            data[i-1] = AnalogReadAvg(analogInPin, NUM_SAMPLES);
            continue;
        }

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
            data[dev_id-1] = atoi(lst[2]);
        }
    }
}


void get_O()
{
    // turn the lights off
    // the others
    Wire.beginTransmission(0);
    Wire.write("q 0");
    Wire.endTransmission();
    // my own
    analogWrite(analogOutPin, 0);

    delay(2000);    // wait for others to turn off and stabilize

    //TODO call 
    get_all_readings(O_vals, 3);

    #ifdef DEBUG
    short i;
    Serial.print("O_vals: ");
    for(i=0; i<3; i++)
    {
        Serial.print(O_vals[i]);
        Serial.print(" ");
    }
    Serial.println("");
    #endif
}


/* Get the E matrix. This should only be called after get_O(). This is because
 * Eij must be compensated by Oj. */
void get_E()
{
    short i;
    char *lst[BUF_SPLIT_LEN];
    short dev_id;
    short numwords;

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
        {
            analogWrite(analogOutPin, 255);
            delay(1000);
            get_all_readings(E_vals[i-1], 3);
            analogWrite(analogOutPin, 0);
            delay(1000);
            continue;
        }

        // turn LED on full power
        Wire.beginTransmission(i);
        Wire.write("q 255");
        Wire.endTransmission();

        delay(1000);    // wait to settle

        get_all_readings(E_vals[i-1], 3);

        Wire.beginTransmission(i);
        Wire.write("q 0");
        Wire.endTransmission();

        delay(1000);    // wait to settle
    }

    // compensate Eij by Oj
    for(i=0; i<3; i++)
        for(int j=0; j<3; j++)
            if(E_vals[i][j] < O_vals[j])
                E_vals[i][j] = 0;
            else
                E_vals[i][j] -= O_vals[j];

    #ifdef DEBUG
    short j;
    Serial.println("E_vals: ");
    for(i=0; i<3; i++)
    {
        for(j=0; j<3; j++)
        {
            Serial.print(E_vals[i][j]);
            Serial.print(" ");
        }
        Serial.println("");
    }
    #endif
}
