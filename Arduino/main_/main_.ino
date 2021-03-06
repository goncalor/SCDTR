#include "utils.h"
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <EEPROM.h>
#include <Wire.h>
#include <stdio.h>
#include <stdlib.h>

//#define DEBUG
#define SNIFF

#define BUF_LEN        80
#define BUF_LEN_2      10
#define BUF_WIRE_LEN   40
#define BUF_SPLIT_LEN  20
#define BUF_STATS_LEN  40   //TODO change to 60? be careful with low memory
#define BAUDRATE 19200
#define SAMPLE_TIME 5000    // microseconds
#define GAIN_K 10           // proportional gain
#define GAIN_D 0.01         // differential gain
#define GAIN_I 10           // integral gain
#define GAIN_FEEDFORWARD 0.1
#define GAIN_ANTIWINDUP 0.05
#define PID_A 10
#define RESISTENCIA 10000.0
#define LDR_B 4.7782
#define LDR_A 0.6901
#define INTERRUPT_TIME (65536 - (16000000./8)*(SAMPLE_TIME/1000000.))
#define EEPROM_ID_ADDRESS 0   // the address for this Arduino's ID
#define NUM_SAMPLES 3   // number of samples used for average of analogRead
#define MASTER_ID 1  // the default ID of the master
#define STATS_PERIOD ((int)((6./BUF_STATS_LEN)/(SAMPLE_TIME/1000000.)))   // stats will be buffered every SAMPLE_TIME * STATS_PERIOD.
#define ILLUM_FREE      15.0   // minimum illuminance for non-occupied desk
#define ILLUM_OCCUPIED  30.0   // minimum illuminance for occupied desk

/* Notes:
 *   - in the Uno floats and doubles are the same
 */

struct circbuf_t {
    volatile float *const buf;
    volatile unsigned head;
    const unsigned maxlen;
};

volatile bool enable_save_stats = true;

void circbuf_add(volatile struct circbuf_t *cb, float data)
{
    // save data to the buffer
    cb->buf[cb->head] = data;

    cb->head++;
    if(cb->head == cb->maxlen)
        cb->head = 0;
}

void circbuf_clear(volatile struct circbuf_t *cb)
{
    unsigned i;

    enable_save_stats = false;
    cb->head = 0;
    for(i=0; i<(cb->maxlen); i++)
        cb->buf[i] = 0;
    enable_save_stats = true;
}

/* Prints the stats buffer 'cb' to serial. The buffer is multiplied by 'scaling'. 'nr_samps_col' should be the current number of samples collected or 0 if that number does not affect scaling for the stats to be printed. */
void stats_print(volatile struct circbuf_t *cb, float scaling, long nr_samps_col)
{
    unsigned i = cb->head;
    nr_samps_col -= nr_samps_col % STATS_PERIOD;  // correct nr samples to a multiple of STATS_PERIOD
    long nr_samps = nr_samps_col - STATS_PERIOD * (BUF_STATS_LEN - 1);

    enable_save_stats = false;
    do
    {
        if(nr_samps <= 0)
            Serial.println(cb->buf[i]*scaling);
        else
            Serial.println(cb->buf[i]*scaling/nr_samps);

        i++;
        nr_samps += STATS_PERIOD;
        if(i == cb->maxlen)
            i = 0;
    }
    while(i != cb->head);
    enable_save_stats = true;
}

uint8_t master_id = MASTER_ID;

void stats_print_i2c(volatile struct circbuf_t *cb, float scaling, long nr_samps_col)
{
    unsigned i = cb->head;
    nr_samps_col -= nr_samps_col % STATS_PERIOD;  // correct nr samples to a multiple of STATS_PERIOD
    long nr_samps = nr_samps_col - STATS_PERIOD * (BUF_STATS_LEN - 1);
    char tmp[12];

    enable_save_stats = false;
    do
    {
        Wire.beginTransmission(master_id);
        if(nr_samps <= 0)
            ftoa(cb->buf[i]*scaling, tmp);
        else
            ftoa(cb->buf[i]*scaling/nr_samps, tmp);
        Wire.write(tmp);
        Wire.endTransmission();
        delay(10);
        i++;
        nr_samps += STATS_PERIOD;
        if(i == cb->maxlen)
            i = 0;
    }
    while(i != cb->head);
    enable_save_stats = true;
}


const int analogInPin = 0;  // Analog input pin that the LDR is attached to
int analogOutPin = 5; // Analog output pin that the LED is attached to

char buf[BUF_LEN];
char buf2[BUF_LEN_2];
char wire_buf[BUF_WIRE_LEN];

extern int O_vals[3];
extern int E_vals[3][3];

bool enable_print = false;
bool occupied = false;
int ctrl_ref = lux_to_pwm(ILLUM_FREE);    // range 0 - 255
int ctrl_verbose_flag= 0;

int wire_my_address;
volatile bool wire_data_available = false;

// PID variables
unsigned int Ts= SAMPLE_TIME;
float gain_k = GAIN_K, gain_d=GAIN_D, ctrl_wind_gain=GAIN_ANTIWINDUP;
float feedforward_gain = GAIN_FEEDFORWARD, a=PID_A, gain_i=GAIN_I;


/* Converts a lux value 'lux_dado' to a PMW duty cycle value (0 to 255).
 * Returns that value. */ 
int lux_to_pwm(float lux_dado) {
    float ctrl_ref_novo=0, ldr=0, tensao=0, resist=RESISTENCIA;

    ldr = pow(lux_dado, -LDR_A) * pow(10.0, LDR_B);
    tensao = 5/(1 + ldr/resist);
    ctrl_ref_novo = tensao*255/5;
    return (int) ceil(ctrl_ref_novo);   // round up, so that minimum will be honoured
}

/* Converts ADC value 'adc_val' (range 0 1023) to a lux value. */
float adc_to_lux(int adc_val) {
    float ldr_ohms, lux;

    ldr_ohms = 1023 * RESISTENCIA/adc_val - RESISTENCIA;
    lux = pow(ldr_ohms, -1/LDR_A)*pow(10, LDR_B/LDR_A);
    return lux;
}


/* Returns an averaged measure of the ADC value at pin 'pin'. The return value
 * will be in the range 0 to 255. */
int AnalogReadAvg(int pin, int n) {
    int i_local;
    int ReadAvg=0;
    for(i_local=0;i_local<n;i_local++)
        ReadAvg+=analogRead(pin);

    return ReadAvg/n;
}


/* read serial into 'buf' until 'buflen' bytes are read or 
   a terminating character is read (NULL, \n, \r) */
int serial_read_str(char *buf, int buflen) {
    int i_local, c;
    if (Serial.available() <= 0) {
        return 0;
    }

    for(i_local=0; i_local<buflen-1; i_local++) {
        c = Serial.read();

        // if a null, \n or \r is found exit
        if (c==0 || c=='\n' || c=='\r')
            break;

        *(buf++) = c;

        if (Serial.available() <= 0) {
            delay(1); // depends of the baudrate
            if (Serial.available() <= 0)
                break;
        }
    }

    *buf='\0';
    return 1;
}


void pwm_config(int freqId) {
    // PWM pins 6 & 5, 9 & 10 or 11 & 3 are controlled by TCCR0B, TCCR1B or TCCR2B
    // freqId 1..5 <=> 32kHz, 4kHz, 500Hz, 125Hz, 31.25Hz
    if (freqId<1 || freqId>5) {
        //Serial.print("ERR: invalid pwm freqId - not in 1..5");
        //Serial.println("E pwm cf");
        Serial.println("E pwm cf");
        return;
    }
    int prescalerVal= 0x07; // clear the last 3bits mask
    TCCR1B &= ~prescalerVal; // configuring pins 9 and 10
    prescalerVal= freqId;
    TCCR1B |= prescalerVal;
}

// control signals
volatile long p, i=0, full_ctrl_u, ctrl_e_sat=0;
volatile int ctrl_e, ctrl_u, ctrl_y=0;
volatile float d=0;

// previous vars
volatile long i_before=0, d_before=0, ctrl_e_before=0;
volatile int y_before = AnalogReadAvg(analogInPin, NUM_SAMPLES);

// Time vars
volatile long end_time, start_time, t0;
volatile long delay_time;

// Precalculated vars
volatile unsigned int ctrl_mapped_ref = map(ctrl_ref, 0, 255, 0, 1023);
volatile float Ts_sec = (float) Ts/1000000.0;
//float gain_i=1./integral_time;

volatile float derivative_const = gain_d /(gain_d + a*Ts_sec);
volatile float gain_k_i = gain_i*gain_k;
volatile float gain_k_a = gain_k * a;
volatile float ref_feedfoward = ctrl_mapped_ref * feedforward_gain;
volatile float Ts_gain_k_i = Ts_sec * gain_k_i;
volatile float gain_d_Ts = gain_d/Ts_sec;

// other
volatile float c;
volatile short print_flag;

// metrics
volatile float energy = 0;
volatile float energy_buf[BUF_STATS_LEN];
volatile short prev_duty = 0;

volatile float confort_error_accum = 0;  // TODO what if the occupation changes?
volatile float confort_error_accum_buf[BUF_STATS_LEN];
volatile float lux_ref = ILLUM_FREE;
volatile float lux_measured = 0;       // lux_measured in t
volatile float lux_measured_t1 = 0;    // lux_measured in t-1
volatile float lux_measured_t2 = 0;    // lux_measured in t-2

volatile float flicker_accum = 0;
volatile float flicker_accum_buf[BUF_STATS_LEN];

volatile unsigned long nr_samples_collected = 0;
volatile unsigned stats_not_saved = 0;

volatile float aux;

volatile circbuf_t energy_cb = {.buf=(float*)energy_buf, .head=0, .maxlen=BUF_STATS_LEN};
volatile circbuf_t confort_cb = {.buf=(float*)confort_error_accum_buf, .head=0, .maxlen=BUF_STATS_LEN};
volatile circbuf_t flicker_cb = {.buf=(float*)flicker_accum_buf, .head=0, .maxlen=BUF_STATS_LEN};


ISR(TIMER1_OVF_vect) {
    TCNT1 = (unsigned int) INTERRUPT_TIME; // reload timer. don't move this
    //start_time = micros();
    print_flag = true;

    ctrl_y = AnalogReadAvg(analogInPin, NUM_SAMPLES);
    ctrl_e = ctrl_mapped_ref - ctrl_y;
    //PID with Anti Windup and feedforward Improved Integral //Lecture  6 pag 32
    p =  (gain_k * ctrl_e);

    //d =  derivative_const * (d_before - gain_k_a * (ctrl_y-y_before));
    d = gain_d_Ts * (ctrl_y-y_before);
    //d=a/(a+Ts_sec)*d-gain_d/(a+Ts_sec)*(ctrl_y-y_before); // % update derivative part

    full_ctrl_u = p + i - d + ref_feedfoward ;	// now add feedforward

    full_ctrl_u = map(full_ctrl_u, 0, 1023, 0, 255); // Doesn't constrain to within range
    ctrl_u = constrain(full_ctrl_u, 0, 255);

    //Serial.println(micros()-start_time);

    // Write Output
    analogWrite(analogOutPin, ctrl_u);

    //Variable updates
    //i_before = i;
    //d_before = d;		
    y_before = ctrl_y;
    ctrl_e_sat = (ctrl_u-full_ctrl_u )*4;
    i =  i + ((ctrl_e + ctrl_e_before) / 2 + ctrl_e_sat * ctrl_wind_gain) * Ts_gain_k_i ;
    ctrl_e_before = ctrl_e; 
    //end_time = micros();

    // compute and save metrics
    // energy
    energy += prev_duty*(SAMPLE_TIME/(float)(255*1000000));
    prev_duty = ctrl_u;

    // confort error
    lux_measured_t2 = lux_measured_t1;
    lux_measured_t1 = lux_measured;
    lux_measured = adc_to_lux(ctrl_y);
    confort_error_accum += lux_ref > lux_measured ? lux_ref - lux_measured : 0;

    // flicker
    aux = lux_measured - 2*lux_measured_t1 + lux_measured_t2;
    flicker_accum += aux > 0 ? aux : -aux;

    if(enable_save_stats)
    {
        if(stats_not_saved == STATS_PERIOD)
        {
            stats_not_saved = 0;
            circbuf_add(&energy_cb, energy);
            circbuf_add(&confort_cb, confort_error_accum);
            circbuf_add(&flicker_cb, flicker_accum);
        }
        else
            stats_not_saved++;
    }

    nr_samples_collected++;
}


// --------------------------------------------------
void config_mode(char *buf) {
    int tmp;
    Serial.print("-- do cnf: ");
    Serial.println(buf);
    switch (buf[0]) {
        case 'v':
            ctrl_verbose_flag= 0;
            break;

        case 'V':
            ctrl_verbose_flag= '1';
            break;

        case 'O':
            // config the analog output pin (where to send the step)
            tmp= buf[1];
            if ('0'<=tmp && tmp<='9')
                tmp= tmp-'0';
            else if ('a'<=tmp && tmp<='d')
                tmp= tmp-'a';
            else {
                Serial.println("E inv outpin");
                return;
            }
            analogOutPin= tmp;

        default:
            Serial.println("E inv modecnf");
    }
}


void serial_process_incoming() {
    float x;
    short dev_id, numwords, aux;
    char *lst[BUF_SPLIT_LEN];
    unsigned long ul;


    #ifdef SNIFF
    Wire.beginTransmission(0);
    Wire.write("z ");
    Wire.write(buf);
    Wire.endTransmission();
    #endif

    // ignore the firts byte (command)
    numwords = split(buf+1, lst, BUF_SPLIT_LEN);

    #ifdef DEBUG
    Serial.print("numwords ");
    Serial.println(numwords);
    Serial.print("lst ");
    for(i=0; i < numwords; i++)
    {
        Serial.print(lst[i]);
        Serial.print(" ");
    }
    Serial.println("");
    #endif


    switch (buf[0]) {

        case 'm':
            master_id = wire_my_address;
            change_master(wire_my_address);
            break;

        case 'F':
            // server asks for calibration
            calibrate();
            // warn the server that the calibration is done
            Serial.println("D");
            break;

        case 'O':
            // ask device for O
            print_O();
            break;

        case 'E':
            // ask device for E
            print_E();
            break;

        case 'l':
            // set lux reference
            // 'l lux [dev_id]'

            if(numwords == 2)
            {
                x = atof(lst[0]);
                dev_id = atoi(lst[1]);
                Wire.beginTransmission(dev_id);
                Wire.write("l ");
                Wire.write(lst[0]);
                //Wire.write(0);
                Wire.endTransmission();
                #ifdef DEBUG
                Serial.print("command for other Arduino.\nID:");
                Serial.println(dev_id);
                Serial.println(x);
                #endif
                break;
            }

            #ifdef DEBUG
            Serial.println("command for me");
            #endif
            x = atof(lst[0]);
            set_reference_lux(x);
            break;

        case 'T':
            // define the sampling period
            // 'T sampleperiod'
            Ts= atoi(lst[0]);
            Ts_sec = (float) Ts/1000000.0;
            Serial.print("Ts[us]=");
            Serial.println(Ts);
            break;

        case 'R':
            // set reference
            // 'R pwmref'
            x = atof(lst[0]);
            set_reference_pwm(x);
            sprintf(buf, "ref=");
            itoa(ctrl_ref, buf2, 10);
            strcat(buf, buf2);
            strcat(buf, "\n");
            Serial.print(buf);
            break;

        case 'c':
            // config the controller
            // 'c x parameter'
            switch (buf[2]) {
                case '0': 
                    x= atof(lst[1]); 
                    noInterrupts();
                    gain_k= x;
                    gain_k_i = gain_i*gain_k;
                    gain_k_a = gain_k * a;
                    Ts_gain_k_i = Ts_sec * gain_k_i;
                    interrupts();

                    break;
                case '1': 
                    noInterrupts();
                    x= atof(lst[1]); 
                    gain_i= x; 
                    gain_k_i = gain_i*gain_k;
                    Ts_gain_k_i = Ts_sec * gain_k_i;
                    interrupts();
                    break;
                case '2': 
                    noInterrupts();
                    x= atof(lst[1]); 
                    gain_d= x; 
                    derivative_const = gain_d /(gain_d + a*Ts_sec);
                    gain_d_Ts = gain_d/Ts_sec;
                    interrupts();
                    break;
                case '3': 
                    x= atof(lst[1]); 
                    noInterrupts();
                    feedforward_gain = x;
                    ref_feedfoward = ctrl_mapped_ref * feedforward_gain; 
                    interrupts();
                    break;
                case '4': 
                    x= atof(lst[1]);
                    noInterrupts();
                    ctrl_wind_gain = x; 
                    interrupts();
                    break;
                case '5': 
                    x= atof(lst[1]); 
                    noInterrupts();
                    a = x; 
                    gain_k_a = gain_k * a;
                    derivative_const = gain_d /(gain_d + a*Ts_sec);
                    interrupts();
                    break;

                default:
                    x=0.0;
                    Serial.print('E');
            }
            Serial.print("x="); Serial.println(x);
            break;

        case 'C':
            // get the current controller
            Serial.print("Proportional gain: ");Serial.println(gain_k);
            Serial.print("Integral gain: ");Serial.println(gain_i);
            Serial.print("Differential gain: "); Serial.println(gain_d);
            Serial.print("Feedforward gain: "); Serial.println(feedforward_gain);
            Serial.print("AntiWindup gain: "); Serial.println(ctrl_wind_gain);
            Serial.print("Filter gain A: "); Serial.println(a);

            break;

        case 'i':
            // read sensor, both in PWM and lux
            {
                int ch= 0;
                if(buf[1]!='\0')
                    ch= buf[1]-'0';
                int v = analogRead(ch);

                Serial.print("i = ");
                Serial.println(v);		
                Serial.print("lux = ");
                Serial.println(adc_to_lux(v));		
            }
            break;

        case 'p':
            // pwm config
            if (buf[1]>='1' & buf[1]<='5')
                pwm_config(buf[1]-'0');
            else
                buf[0]= 'E';
            Serial.print(buf);
            break;

        case 'D':
            // delay in millisec
            t0= millis() + (unsigned long)atoi(&buf[2]);	
            while (millis() < t0)
                ; /* do nothing */
            Serial.print(buf);
            break;

        case 'P':
            // toggle Print
            //Serial.println("\ntime ctrl_u ctrl_y ctrl_e lux");
            Serial.println("\ntime ctrl_e");
            enable_print = !enable_print;
            break;

        case 'g':
            // 'g l|d|o|L|O|r|p|e|v dev_id'
            if(numwords != 2)
            {
                Serial.println("inv");
                break;
            }

            #ifdef DEBUG
            Serial.println("some g command");
            #endif

            dev_id = atoi(lst[1]);
            switch(lst[0][0]) {
                case 'l':
                    // ask device for its illuminance
                    if(dev_id == wire_my_address)
                    {
                        ftoa(adc_to_lux(AnalogReadAvg(analogInPin, NUM_SAMPLES)), buf2);
                        Serial.println(buf2);
                        break;
                    }
                    Wire.beginTransmission(dev_id);
                    Wire.write('f');
                    Wire.endTransmission();
                    while(!wire_data_available)
                        ;   // wait for data
                    wire_data_available = false;
                    Serial.println(wire_buf);
                    break;

                case 'd':
                    // ask device for current duty cycle
                    if(dev_id == wire_my_address)
                    {
                        disable_controller();
                        aux = ctrl_u;
                        enable_controller();
                        Serial.println(aux/255.);
                        break;
                    }
                    Wire.beginTransmission(dev_id);
                    Wire.write('g');
                    Wire.endTransmission();
                    while(!wire_data_available)
                        ;   // wait for data
                    wire_data_available = false;
                    Serial.println(wire_buf);
                    break;

                case 'L':
                    // ask device for desired illuminance
                    if(dev_id == wire_my_address)
                    {
                        //disable_controller();
                        Serial.println(lux_ref);
                        //enable_controller();
                        break;
                    }
                    Wire.beginTransmission(dev_id);
                    Wire.write('h');
                    Wire.endTransmission();
                    while(!wire_data_available)
                        ;   // wait for data
                    wire_data_available = false;
                    Serial.println(wire_buf);
                    break;

                case 'r':
                    // ask device for current reference in lux
                    if(dev_id == wire_my_address)
                    {
                        disable_controller();
                        Serial.println(adc_to_lux(ctrl_u*4));
                        enable_controller();
                        break;
                    }
                    Wire.beginTransmission(dev_id);
                    Wire.write('i');
                    Wire.endTransmission();
                    while(!wire_data_available)
                        ;   // wait for data
                    wire_data_available = false;
                    Serial.println(wire_buf);
                    break;

                case 'p':
                    // ask device for instantaneous power consumption
                    if(dev_id == wire_my_address)
                    {
                        disable_controller();
                        Serial.println(ctrl_u/255.);
                        enable_controller();
                        break;
                    }
                    Wire.beginTransmission(dev_id);
                    Wire.write('j');
                    Wire.endTransmission();
                    while(!wire_data_available)
                        ;   // wait for data
                    wire_data_available = false;
                    Serial.println(wire_buf);
                    break;

                case 'e':
                    // ask device for energy since restart
                    if(dev_id == wire_my_address)
                    {
                        disable_controller();
                        Serial.println(energy);
                        enable_controller();
                        break;
                    }
                    Wire.beginTransmission(dev_id);
                    Wire.write('k');
                    Wire.endTransmission();
                    while(!wire_data_available)
                        ;   // wait for data
                    wire_data_available = false;
                    Serial.println(wire_buf);
                    break;

                case 'c':
                    // ask device for accumulated confort error since restart
                    if(dev_id == wire_my_address)
                    {
                        disable_controller();
                        Serial.println(confort_error_accum/nr_samples_collected);
                        enable_controller();
                        break;
                    }
                    Wire.beginTransmission(dev_id);
                    Wire.write('l');
                    Wire.endTransmission();
                    while(!wire_data_available)
                        ;   // wait for data
                    wire_data_available = false;
                    Serial.println(wire_buf);
                    break;

                case 'v':
                    // ask device for accumulated flicker since restart
                    if(dev_id == wire_my_address)
                    {
                        disable_controller();
                        Serial.println(flicker_accum / (nr_samples_collected * (SAMPLE_TIME/1000000.) * (SAMPLE_TIME/1000000.)));
                        enable_controller();
                        break;
                    }
                    Wire.beginTransmission(dev_id);
                    Wire.write('m');
                    Wire.endTransmission();
                    while(!wire_data_available)
                        ;   // wait for data
                    wire_data_available = false;
                    Serial.println(wire_buf);
                    break;

                    // TODO: improve this
                case 'O':
                    // ask device for external illuminance
                    // if(dev_id == wire_my_address)
                    // {
                    //     Serial.println(adc_to_lux(O_vals[wire_my_address]));
                    //     break;
                    // }
                    // Wire.beginTransmission(dev_id);
                    // Wire.write('n');
                    // Wire.endTransmission();
                    // while(!wire_data_available)
                    //     ;   // wait for data
                    // wire_data_available = false;
                    // Serial.println(wire_buf);
                    Serial.println(adc_to_lux(O_vals[dev_id]));
                    break;

                case 'o':
                    // ask device for occupation status
                    if(dev_id == wire_my_address)
                    {
                        Serial.println(occupied ? "true" : "false");
                        break;
                    }
                    Wire.beginTransmission(dev_id);
                    Wire.write('o');
                    Wire.endTransmission();
                    while(!wire_data_available)
                        ;   // wait for data
                    wire_data_available = false;
                    Serial.println(wire_buf);
                    break;
            }

            break;  // case g

        case 's':
            // set occupation of some desk
            // 's dev_id 0|1'
            if(numwords != 2)
                break;

            #ifdef DEBUG
            Serial.println("s command");
            #endif

            dev_id = atoi(lst[0]);
            aux = atoi(lst[1]);
            if(aux == 0)
                aux = false;
            else if(aux == 1)
                aux = true;
            else
                break;

            if(dev_id == wire_my_address)
            {
                occupied = aux;
                if(occupied)
                    set_reference_lux(ILLUM_OCCUPIED);
                else
                    set_reference_lux(ILLUM_FREE);
                //Serial.println("ack");
                break;
            }
            Wire.beginTransmission(dev_id);
            if(aux)
                Wire.write('r');    // occupied
            else
                Wire.write('s');    // free
            Wire.endTransmission();
            //Serial.println("ack");
            break;

        case 'r':
            #ifdef DEBUG
            Serial.println("system reset");
            #endif
            Wire.beginTransmission(dev_id);
            Wire.write('v');
            Wire.endTransmission();
            //state_reset();
            soft_reset();
            break;

        case 'e':
            // send stored energy
            if(numwords != 1)
            {
                Serial.println("inv");
                break;
            }
            dev_id = atoi(lst[0]);

            if(dev_id == wire_my_address)
            {
                stats_print(&energy_cb, 1., 0);
                break;
            }
            Wire.beginTransmission(dev_id);
            Wire.write('w');
            Wire.endTransmission();

            for(aux=0; aux<BUF_STATS_LEN; aux++)
            {
                while(!wire_data_available)
                    ;   // wait for data
                wire_data_available = false;
                Serial.println(wire_buf);
            }
            break;

        case 'n':
            // send stored confort
            if(numwords != 1)
            {
                Serial.println("inv");
                break;
            }
            dev_id = atoi(lst[0]);

            if(dev_id == wire_my_address)
            {
                disable_controller();
                ul = nr_samples_collected;
                enable_controller();
                stats_print(&confort_cb, 1., ul);
                break;
            }
            Wire.beginTransmission(dev_id);
            Wire.write('x');
            Wire.endTransmission();

            for(aux=0; aux<BUF_STATS_LEN; aux++)
            {
                while(!wire_data_available)
                    ;   // wait for data
                wire_data_available = false;
                Serial.println(wire_buf);
            }
            break;

        case 'f':
            // send stored flicker
            if(numwords != 1)
            {
                Serial.println("inv");
                break;
            }
            dev_id = atoi(lst[0]);

            if(dev_id == wire_my_address)
            {
                disable_controller();
                ul = nr_samples_collected;
                enable_controller();
                stats_print(&flicker_cb, 1./((SAMPLE_TIME/1000000.) * (SAMPLE_TIME/1000000.)), ul);
                break;
            }
            Wire.beginTransmission(dev_id);
            Wire.write('y');
            Wire.endTransmission();

            for(aux=0; aux<BUF_STATS_LEN; aux++)
            {
                while(!wire_data_available)
                    ;   // wait for data
                wire_data_available = false;
                Serial.println(wire_buf);
            }
            break;

        case 'h':
            // use parameters given by simplex
            // 'h pwm1 pwm2 pwm3'
            if(numwords != 3)
            {
                Serial.println("inv");
                break;
            }

            disable_all_controllers();
            for(aux=1; aux<=3; aux++)
            {
                if(aux == wire_my_address)
                {
                    analogWrite(analogOutPin, atoi(lst[aux-1]));
                    continue;
                }
                Wire.beginTransmission(aux);
                Wire.write("q ");
                Wire.write(lst[aux-1]);
                Wire.endTransmission();
            }
            delay(2);
            enable_all_controllers();
            break;

        default:
            Serial.println("inv");
    }
}


void wireReceiveEvent(int nbytes) {
    int i=0;
    char c;
    digitalWrite(13, HIGH);
    #ifdef DEBUG
    Serial.print("hi nbytes ");
    Serial.println(nbytes);
    #endif
    while (Wire.available() && i<nbytes) {
        c = Wire.read();
        wire_buf[i++] = c;
        if(c = '\0') {
            //Serial.println("0!");
            break;
        }
    }
    wire_buf[i] = 0;   // terminate the buffer
    wire_data_available = true;
    digitalWrite(13, LOW);
}


// --------------------------------------------------
void setup() {
    noInterrupts();
    // start serial port at BAUDRATE bps:
    Serial.begin(BAUDRATE);
    pinMode(analogInPin, INPUT);
    pinMode(analogOutPin, OUTPUT);

    pinMode(13, OUTPUT);
    digitalWrite(13, LOW);

    // initialise wire (I2C)
    #ifdef DEBUG
    Serial.print("wire_my_address ");
    Serial.println(EEPROM.read(EEPROM_ID_ADDRESS));
    #endif
    wire_my_address = EEPROM.read(EEPROM_ID_ADDRESS);
    Wire.begin(wire_my_address);
    /* The upper 7 bits are the address to which the 2-wire Serial Interface
     * will respond when addressed by a Master.  If the LSB is set, the TWI
     * will respond to the general call address (0x00), otherwise it will
     * ignore the general call address. */
    TWAR = (wire_my_address << 1) | 1;  // enable broadcasts to be received
    Wire.onReceive(wireReceiveEvent);

    circbuf_clear(&energy_cb);
    circbuf_clear(&confort_cb);
    circbuf_clear(&flicker_cb);
    //if(wire_my_address == master_id)
    //{
    //    calibrate();
    //}
    // warn the server that the device is ready
    Serial.println("D");

    // save the starting time, to be used in graphs
    t0 = micros();

    // setup interrupts
    // reset timer control registers
    TCCR1A = 0;
    TCCR1B = 0;

    TCCR1B |= (1 << CS11); // 8 prescaler
    //TCCR1B |= (1 << CS11) | (1 << CS10); // 64 prescaler
    //TCCR1B |= (1 << CS12) | (1 << CS10); // 1024 prescaler
    TCNT1 = (unsigned int) INTERRUPT_TIME; // preload timer
    TIMSK1 |= (1 << TOIE1); // enable timer overflow interrupt
    interrupts(); // enable all interrupts
}


void enable_controller()
{
    TCNT1 = (unsigned int) INTERRUPT_TIME; // preload timer
    TIMSK1 |= (1 << TOIE1); // enable timer overflow interrupt
}


void disable_controller()
{
    TIMSK1 &= ~(1 << TOIE1); // disable timer overflow interrupt
}


void loop() {
    bool serial_data_available;

    if(wire_data_available)
    {
        wire_process_incoming(wire_buf);
    }

    serial_data_available = serial_read_str(buf, BUF_LEN);

    if(serial_data_available)
    {
        #ifdef DEBUG
        Serial.println("serial data available");
        Serial.println(buf);
        #endif
        serial_process_incoming();
    }

    if(print_flag)
    {
        print_flag = 0;
        //Serial.println(confort_error_accum / nr_samples_collected);
        //Serial.print(lux_measured);
        //Serial.print(" ");
        //Serial.print(lux_measured_t1);
        //Serial.print(" ");
        //Serial.print(lux_measured_t2);
        //Serial.print(" ");
        //Serial.println(flicker_accum / (nr_samples_collected * (SAMPLE_TIME/1000000.) * (SAMPLE_TIME/1000000.)));
        //Serial.print(nr_samples_collected);
        //Serial.print(" ");
        //Serial.println(energy);
        //Serial.println(confort_error_accum);
    }

    if(print_flag and enable_print) {
        print_flag = 0;

        // Prints   
        Serial.print(micros()-t0);
        Serial.print(" ");
        Serial.print(ctrl_mapped_ref);
        Serial.print(" ");
        Serial.println(ctrl_e);

        /*
        // Prints 
        //start_time = micros();
        Serial.print(micros()-t0);
        Serial.print(" ");
        Serial.print(ctrl_u);
        Serial.print(" ");
        Serial.print(ctrl_y);
        Serial.print(" ");
        Serial.print(ctrl_e);
        //Serial.print(" ");
        //Serial.print(p);
        //Serial.print(" ");
        //Serial.print(i);
        //Serial.print(" ");
        //Serial.print(d);
        //Serial.print(" ");
        //Serial.print(ctrl_e_sat);
        Serial.print(" ");
        Serial.println(adc_to_lux(ctrl_y));		
        //Serial.print(" ");
        //Serial.print(ctrl_mapped_ref);
        //Serial.print(" ");
        //end_time = micros();
        //Serial.println(end_time-start_time);
        //Serial.print(",\t");
         */
    }
}


void set_reference_lux(float lux)
{
    disable_controller();
    lux_ref = lux;
    ctrl_ref = lux_to_pwm(lux);
    ctrl_mapped_ref = map(ctrl_ref, 0, 255, 0, 1023);
    ref_feedfoward = ctrl_mapped_ref * feedforward_gain;
    enable_controller();
}

void set_reference_pwm(unsigned short pwm)
{
    disable_controller();
    lux_ref = adc_to_lux(4*pwm);
    ctrl_ref = pwm;
    ctrl_mapped_ref = map(ctrl_ref, 0, 255, 0, 1023);
    ref_feedfoward = ctrl_mapped_ref * feedforward_gain;
    enable_controller();
}


// Function Pototype
void wdt_init(void) __attribute__((naked)) __attribute__((section(".init3")));

// Function Implementation
void wdt_init(void)
{
    MCUSR = 0;
    wdt_disable();
    return;
}

void soft_reset()        
{
    wdt_enable(WDTO_15MS);
    while(1)
        ;
}


/* Reset 'all variables' mainly those that depend on time. */
void state_reset()
{
    nr_samples_collected = 0;
    energy = 0;
    confort_error_accum = 0;
    flicker_accum = 0;
    stats_not_saved = 0;
    set_reference_lux(ILLUM_FREE);
    occupied = false;
    energy_cb.head = 0;
    confort_cb.head = 0;
    flicker_cb.head = 0;
    lux_measured = 0;
    lux_measured_t1 = 0;
    lux_measured_t2 = 0;
    prev_duty = 0;
    enable_save_stats = true;
    //O_vals[3] = {3, 1, 3} ;
    //E_vals[3][3] = { {722, 116, 46}, {592, 698, 99}, {304, 579, 664} };
}
