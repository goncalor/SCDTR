#include "utils.h"
#include <avr/interrupt.h>
#include <EEPROM.h>
#include <Wire.h>
#include <stdio.h>
#include <stdlib.h>

#define DEBUG

#define BUF_LEN 100
#define BUF_LEN_2 10
#define BUF_WIRE_LEN    40
#define BUF_SPLIT_LEN   20
#define BAUDRATE 38400
#define SAMPLE_TIME 5000    // microseconds
#define GAIN_K 10
#define GAIN_D 0.01
#define GAIN_I 10
#define GAIN_FEEDFORWARD 0.1
#define GAIN_ANTIWINDUP 0.05
#define FEEDFORWARD_GAIN 0.1
#define PID_A 10
#define RESISTENCIA 10000.0
#define LDR_B 4.7782
#define LDR_A 0.6901
#define INT_GAIN 10
#define INTERRUPT_TIME (65536 - (16000000./8)*(SAMPLE_TIME/1000000.))
#define EEPROM_ID_ADDRESS 0   // the address for this Arduino's ID
#define NUM_SAMPLES 3   // number of samples used for average of analogRead
#define MASTER_ID 1  // the ID of the master TODO


const int analogInPin = 0;  // Analog input pin that the LDR is attached to
int analogOutPin = 5; // Analog output pin that the LED is attached to

char buf[BUF_LEN];
char buf2[BUF_LEN_2];
char wire_buf[BUF_WIRE_LEN];

int n;

bool enable_print = false;
bool i_am_master = false;
int ctrl_ref=127;	// range 0 - 255
int loopOutputFlag= 1;
int ctrl_verbose_flag= 0;

int wire_my_address;
volatile bool wire_data_available = false;

// PID variables
unsigned int Ts= SAMPLE_TIME;
double gain_k = GAIN_K, gain_d=GAIN_D, ctrl_wind_gain=GAIN_ANTIWINDUP;
double feedforward_gain = GAIN_FEEDFORWARD, a=PID_A, gain_i=GAIN_I;


/* Converts a lux value 'lux_dado' to a PMW duty cycle value (0 to 255).
 * Returns that value. */ 
int lux_to_pwm(double lux_dado) {
    double ctrl_ref_novo=0, ldr=0, voltagem=0, resist=RESISTENCIA;
    ldr = pow(10.0, (-LDR_A*log10(lux_dado) + LDR_B));
    voltagem = 5/(1 + ldr/resist);
    ctrl_ref_novo = voltagem*255/5;
    return ctrl_ref_novo;
}

/* Converts ADC value 'adc_val' (range 0 1023) to a lux value. */
double adc_to_lux(int adc_val) {
    double ldr_ohms, lux;
    ldr_ohms = 1023 * RESISTENCIA/adc_val - RESISTENCIA;
    lux = pow(ldr_ohms, -1/LDR_A)*pow(10, LDR_B/LDR_A);
    return lux;
}


int AnalogReadAvg(int pin,int n) {
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
volatile double d=0;

// previous vars
volatile long i_before=0, d_before=0, ctrl_e_before=0;
volatile int y_before = AnalogReadAvg(analogInPin, NUM_SAMPLES);

// Time vars
volatile long end_time, start_time, t0;
volatile long delay_time;

// Precalculated vars
volatile unsigned int ctrl_mapped_ref = map(ctrl_ref, 0, 255, 0, 1023);
volatile double Ts_sec = (float) Ts/1000000.0;
//double gain_i=1./integral_time;

volatile double derivative_const = gain_d /(gain_d + a*Ts_sec);
volatile double gain_k_i = gain_i*gain_k;
volatile double gain_k_a = gain_k * a;
volatile double ref_feedfoward = ctrl_mapped_ref * feedforward_gain;
volatile double Ts_gain_k_i = Ts_sec * gain_k_i;
volatile double gain_d_Ts = gain_d/Ts_sec;

// other
volatile double c;
volatile short print_flag;

// metrics
volatile double energy = 0;
volatile float prev_duty = 0;

volatile double confort_error_accum = 0;  // TODO what if the occupation changes?
volatile double lux_ref = adc_to_lux(4*ctrl_ref);
volatile double lux_measured;

volatile unsigned long nr_samples_collected = 0;


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
    energy += prev_duty*SAMPLE_TIME/(float)(255*1000000);
    prev_duty = ctrl_u;

    // confort error
    lux_measured = adc_to_lux(ctrl_y);
    confort_error_accum += lux_ref > lux_measured ? lux_ref - lux_measured : 0;


    nr_samples_collected++;
}


// --------------------------------------------------
void config_mode(char *buf) {
    int tmp;
    Serial.print("-- do cnf: ");
    Serial.println(buf);
    switch (buf[0]) {

        case 'f':
            loopOutputFlag= 0; break;
        case 'F':
            loopOutputFlag= 1; break;

        case 'v':
            ctrl_verbose_flag= 0; break;
        case 'V':
            ctrl_verbose_flag= '1'; break;

        case 'O':
            // config the analog output pin (where to send the step)
            tmp= buf[1];
            if ('0'<=tmp && tmp<='9')
                tmp= tmp-'0';
            else if ('a'<=tmp && tmp<='d')
                tmp= tmp-'a';
            else {
                Serial.println('E inv outpin');
                return;
            }
            analogOutPin= tmp;

        default:
            Serial.println("E inv modecnf");
    }
}


void main_send_end_cmd() {
    Serial.print("> ");
}


void main_switch() {
    double x;
    short dev_id;
    char *lst[BUF_SPLIT_LEN];
    short numwords;

    //TODO: take out this if
    if(true) {

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
                i_am_master = true;
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

                Serial.println("command for me");
                x = atof(lst[0]);
                noInterrupts();
                lux_ref = x;
                ctrl_ref = lux_to_pwm(x);
                ctrl_mapped_ref = map(ctrl_ref, 0, 255, 0, 1023);
                ref_feedfoward = ctrl_mapped_ref * feedforward_gain;
                interrupts();   
                break;

            case 'T':
                // define the sampling period
                // 'T sampleperiod'
                Ts= atoi(lst[0]);
                Ts_sec = (float) Ts/1000000.0;
                Serial.print("Ts[us]=");
                Serial.println(Ts);
                break;

            case 'r':
                // set reference
                // 'r pwmref'
                x = atof(lst[0]);
                noInterrupts();
                lux_ref = adc_to_lux(4*x);
                ctrl_ref = x;
                ctrl_mapped_ref = map(ctrl_ref, 0, 255, 0, 1023);
                ref_feedfoward = ctrl_mapped_ref * feedforward_gain;
                interrupts();
                sprintf(buf, "ref=");
                itoa(ctrl_ref, buf2, 10);
                strcat(buf, buf2);
                strcat(buf, "\n");
                //Serial.print(buf);
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
                // read sensor
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
                // delay in milisec
                t0= millis() + (unsigned long)atoi(&buf[2]);	
                while (millis() < t0)
                    ; /* do nothing */
                Serial.print(buf);
                break;

            case 'P':
                // Toggle Print
                //Serial.println("\ntime ctrl_u ctrl_y ctrl_e lux");
                Serial.println("\ntime ctrl_e");
                enable_print = !enable_print;
                break;

            default:
                strcat(buf, " <inv cmd\n");
                Serial.print(buf);
        }

        //Serial.print("> ");
        //main_send_end_cmd();
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
        if(c = '\0')
            Serial.println("0!");
        //    break;
    }
    wire_buf[i] = 0;   // terminate the buffer
    wire_data_available = true;
    digitalWrite(13, LOW);
}


// --------------------------------------------------
void setup() {
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

    if(wire_my_address == MASTER_ID)
    {
        i_am_master = true;
        calibrate();
    }


    // save the starting time, to be used in graphs
    t0 = micros();

    // setup interrupts
    noInterrupts();
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
        main_switch();
        #ifdef DEBUG
        Serial.println("serial data available");
        Serial.println(buf);
        #endif
    }

    //Serial.println(confort_error_accum / nr_samples_collected);
    Serial.println(confort_error_accum);
    delay(1);

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

