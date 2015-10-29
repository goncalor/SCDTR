/*
  Analog input, analog output, serial output
  
  Note: choose monitor to terminate lines (e.g. with "newline"),
  as otherwise string reading need to terminate by timeouts (multiples of 10ms)
 
*/

#include <avr/interrupt.h>

#define SENSORBUF 1
#define BUFSZ 100
#define BUFSZ2 10
#define BAUDRATE 115200
#define SAMPLE_TIME 5000	// microseconds
#define INTERRUPT_TIME 65536 - (16000000/8)*(SAMPLE_TIME/1000000.)
#define INTEGRAL_TIME 0.001
#define GAIN_K 2.0	
#define GAIN_D 0
#define FEEDFORWARD_GAIN 0.1

volatile unsigned long tmp = 0;
volatile unsigned long tmpbuf[BUFSZ];

ISR(TIMER1_OVF_vect)
{
	TCNT1 = (unsigned int) INTERRUPT_TIME; // preload timer
	
	tmp++;
}


const int analogInPin = 0;  // Analog input pin that LDR
//const int analogOutPin = 9; // Analog output pin that the LED is attached to
int analogOutPin = 5; // Analog output pin that the LED is attached to

char buf[BUFSZ];
char buf2[BUFSZ2];

byte ctrl_uArray[SENSORBUF];
int sensorValuesArray[SENSORBUF];
unsigned long t0, t1, timeArray[SENSORBUF];
int n;

int ctrl_ref=127, ctrl_e, ctrl_u, ctrl_y;
//int ctrl_e1, ctrl_e2, ctrl_u1, ctrl_u2;
//double a0=2.0, a1=0.0, a2=0.0, b1=0.0, b2=0.0;
unsigned int Ts= SAMPLE_TIME;
double gain_k = GAIN_K, integral_time=INTEGRAL_TIME, gain_d=GAIN_D, ctrl_wind_gain=1./INTEGRAL_TIME;
double feedforward_gain = FEEDFORWARD_GAIN;
int loopMode= 0, loopOutputFlag= 1;
int ctrl_verbose_flag= 0; //1;

int AnalogReadAvg(int pin,int n){
	int i;
	int ReadAvg=0;
	for(i=0;i<n;i++)
		ReadAvg+=analogRead(pin);
	
	return ReadAvg/n;
}

void serial_print_ctrl_data() {
  for (n=0; n<SENSORBUF; n++) {
    Serial.print(sensorValuesArray[n]);
    Serial.print(" ");
    Serial.print(ctrl_uArray[n]);
    Serial.print(" ");
    Serial.println(timeArray[n]);
  }
}


int serial_read_str(char *buf, int buflen){
/* read till the end of the buffer or a terminating chr*/
  int i, c;
  if (Serial.available() <= 0) {
    return 0;
  }
  for (i=0; i<buflen-1; i++) {
    c= Serial.read();
    if (c==0 || c==0x0A || c==0x0D) break;
    //buf[i]= c;
    *buf++= c;
    //Serial.print((char)c);
    if (Serial.available() <= 0) {
      delay(10); // 10milliseconds = BAUDRATEbits / BAUDRATEbits/sec
      if (Serial.available() <= 0) break;
    }
  }
  //buf[i]= '\0';
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

/*
void ctrl_finite_loop() {
//  ctrl_ref= 127;

  pwm_config(1);

  // ctrl loop:
  t0= micros(); t1= t0+Ts; // 1 msec

  for (n=0; n<200; n+=2, t1= t1+Ts) {
    // while 1, if time
    while (micros()<t1)
      // do nothing
      ;

    //   read sensors
    //sensorValuesArray[n]= analogRead(0);
    timeArray[n]= micros();
    ctrl_y= analogRead(1);
    sensorValuesArray[n+1]= ctrl_y;
    timeArray[n+1]= micros();

    //   calc ctrl
    ctrl_e= ctrl_ref - ctrl_y;
    ctrl_u= a0*ctrl_e +a1*ctrl_e1 +a2*ctrl_e2
                      -b1*ctrl_u1 -b2*ctrl_u2;
    sensorValuesArray[n]= ctrl_u; // save values before truncation
    if (ctrl_u > 255) ctrl_u=255;
    if (ctrl_u < 0) ctrl_u=0;
    analogWrite(analogOutPin, ctrl_u);

    ctrl_u2= ctrl_u1;
    ctrl_u1= ctrl_u;
    ctrl_e2= ctrl_e1;
    ctrl_e1= ctrl_e;
    
    //   read cmd
    //   write cmd
    //   if break loop then break
  }

  // stop the motor
  analogWrite(analogOutPin, 0);
}


void ctrl_infinite_loop() {

  int c;
  pwm_config(1);

  // ctrl loop:
  t0= micros(); t1= t0+Ts; // 1 msec

  for (n=0; n<200; n+=2, t1= t1+Ts) {
    if (n>=198)
      n= 0; // make an infinite loop
    
    // while 1, if time
    while (micros()<t1)
      // do nothing
      ;

    //   read sensors
    switch (loopMode) {

      case 0:
        // the default control mode has a fixed ref
        // just read analog channel 1 (not zero)
        //sensorValuesArray[n]= analogRead(0);
        timeArray[n]= micros();

        ctrl_y= analogRead(1);
        sensorValuesArray[n+1]= ctrl_y;
        timeArray[n+1]= micros();
        break;

      case 9:
        // read two channels, 0=ref, 1=sensor
        // the extra analogRead makes the loop slower
        ctrl_ref= analogRead(0);
        sensorValuesArray[n]= ctrl_ref;
        timeArray[n]= micros();

        ctrl_y= analogRead(1);
        sensorValuesArray[n+1]= ctrl_y;
        timeArray[n+1]= micros();
        break;
    }

    //   calc ctrl
    ctrl_e= ctrl_ref - ctrl_y;
    ctrl_u= a0*ctrl_e +a1*ctrl_e1 +a2*ctrl_e2
                      -b1*ctrl_u1 -b2*ctrl_u2;
    sensorValuesArray[n]= ctrl_u; // save values before truncation
    if (ctrl_u > 255) ctrl_u=255;
    if (ctrl_u < 0) ctrl_u=0;
    analogWrite(analogOutPin, ctrl_u);

    ctrl_u2= ctrl_u1;
    ctrl_u1= ctrl_u;
    ctrl_e2= ctrl_e1;
    ctrl_e1= ctrl_e;
    
    //   read cmd, and write ans cmd
    //   if break loop then break
    
    if (Serial.available() > 0) {
	  c= Serial.read();
      buf2[0]= c;
      buf2[1]= '\0';
      Serial.print(buf2);
      break; // break the loop
    }
  }

  // stop the motor
  analogWrite(analogOutPin, 0);
}
*/
void ctrl_loop() {
	double full_ctrl_u, c, ctrl_e_sat = 0;
	unsigned long write_time;
	unsigned long end_time;
	unsigned long start_time;
	unsigned long t0;
	unsigned long ctrl_ui=0;
	unsigned long ctrl_ui_before=0;
	unsigned int ctrl_mapped_ref;
	long delay_time;
	
	Serial.println("write_t ctrl_u ctrl_y ctrl_e ctrl_ui ctrl_e_sat ctrl_ref");
	ctrl_mapped_ref = map(ctrl_ref, 0, 254, 0, 1023);
	t0=micros();
	while(1){
		start_time = micros();
		ctrl_y = AnalogReadAvg(analogInPin,3);
		ctrl_e = ctrl_mapped_ref - ctrl_y;
		//Proportional
		//full_ctrl_u = gain_k*ctrl_e;
		/*Proportinal-Integral*/
		/*ctrl_ui = ctrl_ui_before + Ts/(integral_time*1000000) * ctrl_e;
		ctrl_ui_before = ctrl_ui;
		full_ctrl_u = gain_k * ctrl_e + gain_k*integral_time * ctrl_ui;
		full_ctrl_u = map(full_ctrl_u, 0, 1023, 0, 255); // Doesn't constrain to within range
		ctrl_u = constrain(full_ctrl_u, 0, 255);*/
		
		/*Proportinal-Integral with Anti Windup*/ // Page307 chapter 10
		/*ctrl_ui = ctrl_ui_before + Ts/(integral_time*1000000) * (ctrl_e+ctrl_e_sat*ctrl_wind_gain);
		ctrl_ui_before = ctrl_ui;
		full_ctrl_u = gain_k * ctrl_e + gain_k*integral_time * ctrl_ui ;
		full_ctrl_u = map(full_ctrl_u, 0, 1023, 0, 255); // Doesn't constrain to within range
		ctrl_u = constrain(full_ctrl_u, 0, 255);
		ctrl_e_sat = full_ctrl_u-ctrl_u;*/
		
		/*Proportinal-Integral with Anti Windup and feedforward*/ // Page307 chapter 10
		ctrl_ui = ctrl_ui_before + Ts/(integral_time*1000000) * (ctrl_e + ctrl_e_sat * ctrl_wind_gain);
		ctrl_ui_before = ctrl_ui;
		full_ctrl_u = (gain_k * ctrl_e) + (gain_k * integral_time * ctrl_ui) + (ctrl_mapped_ref * feedforward_gain);	// now add feedforward
		full_ctrl_u = map(full_ctrl_u, 0, 1023, 0, 255); // Doesn't constrain to within range
		ctrl_u = constrain(full_ctrl_u, 0, 255);
		ctrl_e_sat = full_ctrl_u-ctrl_u;
		

		write_time = micros();	
		analogWrite(analogOutPin, ctrl_u);

		Serial.print(write_time-t0);
		Serial.print(" ");
		Serial.print(full_ctrl_u);
		Serial.print(" ");
		Serial.print(ctrl_y);
		Serial.print(" ");
		Serial.print(ctrl_e);
		Serial.print(" ");
		Serial.print(ctrl_ui);
		Serial.print(" ");
		Serial.print(ctrl_e_sat);
		Serial.print(" ");
		Serial.println(map(ctrl_ref, 0, 254, 0, 1023));
		//Serial.print(",\t");


		if (Serial.available() > 0) {
		  c= Serial.read();
		  buf2[0]= c;
		  buf2[1]= '\0';
		  Serial.print(buf2);
		  analogWrite(analogOutPin, 0); //turn LED off
		  break; // break the loop
		}
		
		end_time = micros();
		delay_time = Ts - (end_time - start_time);
		delayMicroseconds(delay_time);
/*		if(delay_time > 0)
			Serial.println("OK");
		else
			Serial.println("NOT OK");*/
	}

/*
  int c;
  pwm_config(1); // set PWM at max freq

  // ctrl loop:
  t0= micros(); t1= t0+Ts; // 1 msec
  Serial.println("[Sensor CTRL_U TIME(us)]");
  for (n=0; n<SENSORBUF; n++, t1= t1+Ts) {
    // while 1, if time
    while (micros()<t1)
      // do nothing
      ;

    //   read sensors

	// the default control mode has a fixed ref
	// just read analog channel 1 (not zero)
	//sensorValuesArray[n]= analogRead(0);
	timeArray[n]= micros();
	ctrl_y= analogRead(analogInPin);
	sensorValuesArray[n]= ctrl_y;


    //   calc ctrl
    ctrl_e= ctrl_ref - ctrl_y;
    ctrl_u= a0*ctrl_e +a1*ctrl_e1 +a2*ctrl_e2
                      -b1*ctrl_u1 -b2*ctrl_u2;
    ctrl_uArray[n]= ctrl_u; // save values before truncation
    if (ctrl_u > 255) ctrl_u=255;
    if (ctrl_u < 0) ctrl_u=0;
    analogWrite(analogOutPin, ctrl_u);

    ctrl_u2= ctrl_u1;
    ctrl_u1= ctrl_u;
    ctrl_e2= ctrl_e1;
    ctrl_e1= ctrl_e;
    
    //   read cmd, and write ans cmd
    //   if break loop then break
    
    if (Serial.available() > 0) {
	  c= Serial.read();
      buf2[0]= c;
      buf2[1]= '\0';
      Serial.print(buf2);
      break; // break the loop
    }
	
	if (n==SENSORBUF-1){
		serial_print_ctrl_data();
		n= 0; // make an infinite loop
    }
  }

 // Turn the LED off
  analogWrite(analogOutPin, 0);*/
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
  
// read cmd and execute it
  if (serial_read_str(&buf[0], BUFSZ)) {
    //Serial.print("-- Got cmd: ");
    //Serial.println(buf); // echo the command just received

    // switch cmd
    switch (buf[0]) {
      case '\0':
          break;

		  
		case 'L':
				//Start our controler 
				//that starts a flow of data to the serial port
				ctrl_loop();
        break;
      case '?':
        Serial.println("ver 8.10.2015");
        break;
      
      case 'T':
        // define the sampling period
        Ts= atoi(&buf[1]);
        Serial.print("Ts[us]=");
        Serial.println(Ts);
        break;

      case 'r':
        // set reference
        x= atof(&buf[1]);
        ctrl_ref= x;
        sprintf(buf, "ref=");
        itoa(ctrl_ref, buf2, BUFSZ2);
        strcat(buf, buf2); strcat(buf, "\n");
        Serial.print(buf);
        break;
        
      /*case 'm':
        // select the loop mode
        if (buf[1]=='c') {
          config_mode(&buf[2]);
          Serial.print(buf);
          break;
        }
        loopMode= atoi(&buf[1]);
        sprintf(buf, "loopMode=%d\n", loopMode);
        Serial.print(buf);
        break;*/
			
      case 'c':
        // config the controller
        switch (buf[1]) {
          case '0': x= atof(&buf[2]); gain_k= x; break;
          case '1': x= atof(&buf[2]); integral_time= x; break;
          case '2': x= atof(&buf[2]); gain_d= x; break;
          case '3': x= atof(&buf[2]); feedforward_gain = x; break;
          case '4': x= atof(&buf[2]); ctrl_wind_gain = x; break;
 
          default: x=0.0; Serial.print('E');
        }
        Serial.print("x="); Serial.println(x);
        break;
		
      case 'C':
        // get the current controller
        Serial.print("Proportional gain: ");Serial.println(gain_k);
		Serial.print("Integral time: ");Serial.println(integral_time);
		Serial.print("Differential gain: "); Serial.println(gain_d);
		Serial.print("Feedforward gain: "); Serial.println(feedforward_gain);
		Serial.print("AntiWindup gain: "); Serial.println(ctrl_wind_gain);

        break;
		
		
		/* PROF CONTROLER
      case 'c':
        // config the controller
        switch (buf[1]) {
          case '0': x= atof(&buf[2]); a0= x; break;
          case '1': x= atof(&buf[2]); a1= x; break;
          case '2': x= atof(&buf[2]); a2= x; break;
          case '3': x= atof(&buf[2]); b1= x; break;
          case '4': x= atof(&buf[2]); b2= x; break;
          default: x=0.0; Serial.print('E');
        }
        Serial.print("x="); Serial.println(x);
        break;
		
      case 'C':
        // get the current controller
        Serial.print("Cz= (");
        Serial.print(a0); Serial.print(" + ");
        Serial.print(a1); Serial.print(" Z^-1 + ");
        Serial.print(a2); Serial.print(" Z^-2) / (1 + ");
        Serial.print(b1); Serial.print(" Z^-1 + ");
        Serial.print(b2); Serial.println(" Z^-2)");
        break;
		*/
      case 'i':
        // read sensor
        {
        int ch= 0;
        if (buf[1]!='\0') ch= buf[1]-'0';
        int v= analogRead(ch);
        buf[0]= 'i'; buf[1]= ch+'0'; buf[2]= '\0';
        itoa(v, buf2, BUFSZ2); strcat(buf, buf2);
        strcat(buf, "\n");
        Serial.print(buf);
        }
        break;

      case 'o':
        // set actuation
        {
        int v= atoi(&buf[1]);
        analogWrite(analogOutPin, v);
        sprintf(buf, "o");
        itoa(v, buf2, BUFSZ2); strcat(buf, buf2);
        strcat(buf, "\n");
        Serial.print(buf);
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

      /*case 'x':
        // run the (finite time) control loop
        ctrl_finite_loop();
        if (ctrl_verbose_flag)
          serial_print_data_n();
        //Serial.print(buf);
        break;
        
      case 'X':
        main_send_end_cmd(); // reply cmd received
        // run the infinite control loop
        //   end loop with a received message
        ctrl_infinite_loop();
        if (ctrl_verbose_flag)
          serial_print_data_n();
        //Serial.print(buf);
        break;
        
      case '<':
        // get the data buffers
        serial_print_data_n();
        break;*/

      case 'd':
        // digital input/output
        {
          // find the channel number
        int ch= 0;
        if (buf[1]>='0' & buf[1]<='9')
          ch= buf[1]-'0';
        else if (buf[1]>='a' & buf[1]<='d') // d=> dig output 13 (LED)
          ch= buf[1]-'a'+10;
        else if (buf[1]>='A' & buf[1]<='D')
          ch= buf[1]-'A'+10;

        if (buf[2]=='i') {
          // input the digital bit
          pinMode(ch, INPUT);
          if (digitalRead(ch))
            buf[3]= '1';
          else
            buf[3]= '0';
          buf[4]= '\0';
        }
        else if (buf[2]=='o') {
          // output the digital bit
          pinMode(ch, OUTPUT);
          if (buf[3]=='1')
            digitalWrite(ch, HIGH);
          else
            digitalWrite(ch, LOW);
        }
        else
          // do nothing
          buf[0]='E';
        }
        Serial.print(buf);
        break;
        
      case 'D':
        // delay in milisec
        t0= millis() +(unsigned long)atoi(&buf[1]);
        while (millis() < t0)
          /* do nothing */ ;
        Serial.print(buf);
        break;

      default:
        strcat(buf, " <inv cmd\n");
        Serial.print(buf);
    }

    //Serial.print("> ");
    main_send_end_cmd();
  }
}


// --------------------------------------------------
void setup() {
  // start serial port at BAUDRATE bps:
  Serial.begin(BAUDRATE);

  noInterrupts();
  TCCR1A = 0;
  TCCR1B = 0;

  TCCR1B |= (1 << CS11); // 8 prescaler
  TCNT1 = INTERRUPT_TIME; // preload timer 65536-16MHz/8
  TIMSK1 |= (1 << TOIE1); // enable timer overflow interrupt
  interrupts(); // enable all interrupts

  // debug
  pinMode(13, OUTPUT);
}

void loop() {
  //main_switch();

  delay(500);
  //Serial.println(tmpbuf[tmp]);
//  Serial.println(tmp);
  Serial.println(INTERRUPT_TIME);
}

