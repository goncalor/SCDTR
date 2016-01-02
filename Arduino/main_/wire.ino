#include "utils.h"
#include <Wire.h>

#define BUF_SPLIT_LEN 15

int O_vals[3] = {3, 1, 3};
int E_vals[3][3] = { {722, 116, 46}, {592, 698, 99}, {304, 579, 664} };


void wire_process_incoming(char *str)
{
    char *lst[BUF_SPLIT_LEN];
    short numwords;
    float fl;
    int in;
    unsigned long ul;
    char itoabuf[10];

    wire_data_available = false;

    #ifdef DEBUG
    Serial.print("incoming wire str: ");
    Serial.println(str);
    #endif

    numwords = split(str+1, lst, BUF_SPLIT_LEN);

    switch(str[0])
    {
        case 'a':
            // reply to master with the value on the LDR
            // reply to send 'b my_address ldr'
            Wire.beginTransmission(master_id);
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
            Wire.beginTransmission(master_id);
            ftoa(adc_to_lux(AnalogReadAvg(analogInPin, NUM_SAMPLES)), itoabuf);
            Wire.write(itoabuf);
            Wire.endTransmission();
            break;

        case 'g':
            // reply to master with the current duty cycle
            Wire.beginTransmission(master_id);
            disable_controller();
            in = ctrl_u;
            enable_controller();
            ftoa(in/255., itoabuf);
            Wire.write(itoabuf);
            Wire.endTransmission();
            break;

        case 'h':
            // reply to master with the desired illuminance
            Wire.beginTransmission(master_id);
            //noInterrupts();
            ftoa(lux_ref, itoabuf);
            Wire.write(itoabuf);
            //interrupts();
            Wire.endTransmission();
            break;

        case 'i':
            // reply to master with current reference in lux
            Wire.beginTransmission(master_id);
            disable_controller();
            in = ctrl_u;
            enable_controller();
            ftoa(adc_to_lux(in*4), itoabuf);
            Wire.write(itoabuf);
            Wire.endTransmission();
            break;

        case 'j':
            // reply to master with instanteneous power consumption
            Wire.beginTransmission(master_id);
            disable_controller();
            fl = ctrl_u/255.;
            enable_controller();
            ftoa(fl, itoabuf);
            Wire.write(itoabuf);
            Wire.endTransmission();
            break;

        case 'k':
            // reply to master with energy since restart
            Wire.beginTransmission(master_id);
            disable_controller();
            fl = energy;
            enable_controller();
            ftoa(fl, itoabuf);
            Wire.write(itoabuf);
            Wire.endTransmission();
            break;

        case 'l':
            // reply to master with accumulated confort error since restart
            Wire.beginTransmission(master_id);
            disable_controller();
            fl = confort_error_accum/nr_samples_collected;
            enable_controller();
            ftoa(fl, itoabuf);
            Wire.write(itoabuf);
            Wire.endTransmission();
            break;

        case 'm':
            // reply to master with accumulated flicker since restart
            Wire.beginTransmission(master_id);
            disable_controller();
            ul = nr_samples_collected;
            enable_controller();
            ftoa(flicker_accum / (ul * (SAMPLE_TIME/1000000.) * (SAMPLE_TIME/1000000.)), itoabuf);
            Wire.write(itoabuf);
            Wire.endTransmission();
            break;

        case 'n':
            // reply to master with external illuminance
            Wire.beginTransmission(master_id);
            //disable_controller();
            ftoa(adc_to_lux(O_vals[wire_my_address]), itoabuf);
            Wire.write(itoabuf);
            //enable_controller();
            Wire.endTransmission();
            break;

        case 'o':
            // reply to master with occupation status
            Wire.beginTransmission(master_id);
            //disable_controller();
            Wire.write(occupied ? "true" : "false");
            //enable_controller();
            Wire.endTransmission();
            break;

        case 'p':
            // set the reference in PWM value
            // 'p pwm'
            in = atoi(lst[0]);
            disable_controller();
            ctrl_mapped_ref = map(in, 0, 255, 0, 1023);
            ref_feedfoward = ctrl_mapped_ref * feedforward_gain;
            enable_controller();
            break;

        case 'q':
            // set the LED to a certain PWM value 
            // 'q pwm'
            analogWrite(analogOutPin, atoi(lst[0]));
            break;

        case 'r':
            // set the occupation status to occupied
            // 'r'
            occupied = true;
            set_reference_lux(ILLUM_OCCUPIED);
            break;

        case 's':
            // set the occupation status to occupied
            // 's'
            occupied = false;
            set_reference_lux(ILLUM_FREE);
            break;

        case 't':
            // set reference in lux
            // 't lux'
            fl = atof(lst[0]);
            set_reference_lux(fl);
            break;

        case 'u':
            // a new master has arrived
            // 'u'
            master_id = atoi(lst[0]);
            break;

        case 'v':
            // system reset
            // 'v'
            //state_reset();
            soft_reset();
            break;

        case 'w':
            // reply to master with energy in the last minute
            // 'w'
            circbuf_print_i2c(&energy_cb, 1.);
            break;

        case 'x':
            // reply to master with confort in the last minute
            // 'x'
            disable_controller();
            ul = nr_samples_collected;
            enable_controller();
            circbuf_print_i2c(&confort_cb, 1./ul);
            break;

        case 'y':
            // reply to master with flicker in the last minute
            // 'y'
            disable_controller();
            ul = nr_samples_collected;
            enable_controller();
            circbuf_print_i2c(&flicker_cb, 1./(flicker_accum / (nr_samples_collected * (SAMPLE_TIME/1000000.) * (SAMPLE_TIME/1000000.))));
            break;

            #ifdef SNIFF
        case 'z':
            for(in=0; in < numwords; in++)
            {
                Serial.print(lst[in]);
                Serial.print(" ");
            }
            Serial.println("");
            break;
            #endif

        default:
            break;
    }
}


void change_master(unsigned short new_master_id)
{
    char tmp[4];

    Wire.beginTransmission(0);  // broadcast
    Wire.write("u ");
    itoa(new_master_id, tmp, 10);
    Wire.write(tmp);
    Wire.endTransmission();
}


void calibrate()
{
    change_master(wire_my_address);  // the master has the power to calibrate
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
        if(i == wire_my_address)
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


/* Prints 'O_vals' to serial. */
void print_O()
{
    short i;

    for(i=0; i<3; i++)
    {
        Serial.print(O_vals[i]);
        Serial.print(" ");
    }
    Serial.println("");
}


/* Prints 'E_vals' to serial. */
void print_E()
{
    short i, j;
    for(i=0; i<3; i++)
    {
        for(j=0; j<3; j++)
        {
            Serial.print(E_vals[i][j]);
            Serial.print(" ");
        }
        Serial.println("");
    }
}


/* Get the O matrix. */
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
    get_all_readings(O_vals, 3);

    #ifdef DEBUG
    Serial.print("O_vals: ");
    print_O();
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
        if(i == wire_my_address)
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
    Serial.println("E_vals: ");
    print_E();
    #endif
}
