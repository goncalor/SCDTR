/*
  Analog input, analog output, serial output
 
*/

// These constants won't change.  They're used to give names
// to the pins used:
const int analogInPin = 0;  // Analog input pin that the potentiometer is attached to
const int analogOutPin = 9; // Analog output pin that the LED is attached to

int sensorValue = 0;        // value read from the pot
int outputValue = 0;        // value output to the PWM (analog out)

int sensorValuesArray[200];
unsigned long t0, t1, timeArray[200];
int n;


void setup() {
  // initialize serial communications at 9600 bps:
  Serial.begin(9600); 
}



void serial_print_data(float a0) {
  Serial.print("%----- start time [us]= ");
  Serial.println(t0);
  int exper=a0*2;
  Serial.print("x");
  Serial.print(exper);
  Serial.print("= [");
  for (n=0; n<200; n+=2) {
    Serial.print(" "); //v1=");
    Serial.print(sensorValuesArray[n]);
    Serial.print(" "); //v2=");
    Serial.print(sensorValuesArray[n+1]);
    Serial.print(" "); //t1=");
    Serial.print(timeArray[n]);
    Serial.print(" "); //t2=");
    Serial.println(timeArray[n+1]);
  }
  Serial.println("];");
  
}


void pwm_config(int freqId) {
  // freqId 1..5 <=> 32kHz, 4kHz, 500Hz, 125Hz, 31.25Hz
  if (freqId<1 || freqId>5) {
    Serial.print("ERR: invalid pwm freqId - not in 1..5");
    return;
  }
  int prescalerVal= 0x07; // clear the last 3bits mask
  /* use TCCR1B for Arduino UNO and TCCR2B for Arduino MEGA */
  
  TCCR1B &= ~prescalerVal;
  prescalerVal= freqId;
  TCCR1B |= prescalerVal;

  /*
  TCCR2B &= ~prescalerVal;
  prescalerVal= freqId;
  TCCR2B |= prescalerVal;
  */
}


int freqId= 0;

void pwm_tst() {

  Serial.print("----- freqId= "); Serial.println(freqId);
  if (freqId > 0) pwm_config(freqId);
  freqId++; if (freqId > 5) freqId=1; // loop 1..5
  delay(1000); // 1000 msec = 1 sec
  
  // read the analog in value:
  t0= micros();
  //analogWrite(analogOutPin, 255);           
  analogWrite(analogOutPin, 127);           
  for (n=0; n<200; n+=2) {
    sensorValuesArray[n]= analogRead(0);
    timeArray[n]= micros();
    sensorValuesArray[n+1]= analogRead(1);
    timeArray[n+1]= micros();
  }
  analogWrite(analogOutPin, 0);           
  
  serial_print_data(0);
}


// --------------------------------------------------

void config_loop() {
// config loop:
// read cmd
// switch cmd
}


int ctrl_ref, ctrl_e, ctrl_u, ctrl_y;

#define Ts 1000

void ctrl_loop(double a0) {
  ctrl_ref= 127;
  // double a0= 2.0; // gain of the controller

  pwm_config(1);

  // ctrl loop:
  t0= micros(); t1= t0+Ts; // 1 msec

  for (n=0; n<200; n+=2, t1= t1+Ts) {
    while (micros()<t1)
      // do nothing
      ;

    //   read sensor
    sensorValuesArray[n]= analogRead(0);
    timeArray[n]= micros();
    ctrl_y= analogRead(1);
    sensorValuesArray[n+1]= ctrl_y;
    timeArray[n+1]= micros();

    //   calc ctrl
    ctrl_e= ctrl_ref - ctrl_y;
    ctrl_u= a0*ctrl_e;
    // sensorValuesArray[n]= ctrl_u; // lose saved ctrl_y

    /*
    ctrl_u= a0*ctrl_e +a1*ctrl_e1 +a2*ctrl_e2
              -ctrl_y -b1*ctrl_y1 -b2*ctrl_y2;
    ctrl_y2= ctrl_y1;
    ctrl_y1= ctrl_y;
    ctrl_e2= ctrl_e1;
    ctrl_e1= ctrl_e;
    */

    if (ctrl_u > 255) ctrl_u=255;
    if (ctrl_u < 0) ctrl_u=0;

    //   set DAC
    analogWrite(analogOutPin, ctrl_u);

    //   read cmd
    //   write cmd
    //   if break loop then break
  }

  // stop the motor
  analogWrite(analogOutPin, 0);
}


void loop() {
  double a0;
  // config_loop();
  for (a0=0.5; a0<=3.0; a0=a0+0.5) {
    Serial.print("%----- a0= ");
    Serial.println(a0);
    ctrl_loop(a0);
    serial_print_data(a0);
  }
}

