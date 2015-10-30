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
#define SAMPLE_TIME 5000
#define INTEGRAL_TIME 0.1
#define GAIN_K 1	
#define GAIN_D 0
#define FEEDFORWARD_GAIN 0
#define PID_A 0

const int analogInPin = 0;  // Analog input pin that LDR
//const int analogOutPin = 9; // Analog output pin that the LED is attached to
int analogOutPin = 5; // Analog output pin that the LED is attached to

char buf[BUFSZ];
char buf2[BUFSZ2];

byte ctrl_uArray[SENSORBUF];
int sensorValuesArray[SENSORBUF];
unsigned long t0, t1, timeArray[SENSORBUF];
int n;

int ctrl_ref=127, ctrl_e, ctrl_u, ctrl_y=0;	
unsigned int Ts= SAMPLE_TIME;
double gain_k = GAIN_K, integral_time=INTEGRAL_TIME, gain_d=GAIN_D, ctrl_wind_gain=0.5/INTEGRAL_TIME;
double feedforward_gain = FEEDFORWARD_GAIN, a=PID_A ;
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


void ctrl_loop() {
	
	analogWrite(analogOutPin, 0); //turn LED off
	
	// Global vars
	/*
	gain_d
	gain_k
	integral_time
	ctrl_wind_gain
	feedforward_gain
	a	
	ctrl_ref
	ctrl_y
	
	*/
	
	
	// control signals
	long  p,d,i, full_ctrl_u, ctrl_e_sat=0;

	// previous vars
	long i_before =0 , d_before=0,ctrl_e_before=0;
	unsigned int y_before=0;

	// Time vars
	unsigned long end_time, start_time,t0;
	long delay_time;

	// Precalculated vars
	unsigned int ctrl_mapped_ref = map(ctrl_ref, 0, 255, 0, 1024);
	double Ts_sec = Ts/1000000.0;
	double gain_i=1./integral_time;
	double derivative_const = gain_d /(gain_d + a*Ts_sec);
	double gain_k_i = gain_i*gain_k;
	double gain_k_a = gain_k * a;
	double ref_feedfoward = ctrl_mapped_ref * feedforward_gain;
	
	
	// other
	double c;
	
	
	Serial.println("time full_ctrl_u ctrl_y ctrl_e  p i d ctrl_e_sat ctrl_mapped_ref");
	t0=micros();
		
	while(1){
		start_time = micros();
		ctrl_y = AnalogReadAvg(analogInPin,3);
		ctrl_e = ctrl_mapped_ref - ctrl_y;
		/*PID with Anti Windup and feedforward Improved Integral*/ //Lecture  6 pag 32
		p =  (gain_k * ctrl_e);
		i =  i_before + Ts_sec * ((ctrl_e + ctrl_e_before) / 2 + ctrl_e_sat * ctrl_wind_gain) * gain_k_i ;
		//i =  i_before + Ts_sec * 	(ctrl_e + ctrl_e_sat * ctrl_wind_gain) * gain_k_i ;

		d =  derivative_const * (d_before - gain_k_a * (ctrl_y-y_before));
		full_ctrl_u = p + i + d + ref_feedfoward ;	// now add feedforward
			
		full_ctrl_u = map(full_ctrl_u, 0, 1024, 0, 255); // Doesn't constrain to within range
		ctrl_u = constrain(full_ctrl_u, 0, 255);

		//Serial.println(micros()-start_time);
		
		//Variable updates
		i_before = i;
		d_before = d;		
		ctrl_e_before = ctrl_e;	
		ctrl_e_sat = (ctrl_u-full_ctrl_u )*4;
		
		// Write Output
		analogWrite(analogOutPin, ctrl_u);

		// Prints 
		Serial.print(start_time-t0);
		Serial.print(" ");
		Serial.print(full_ctrl_u);
		Serial.print(" ");
		Serial.print(ctrl_y);
		Serial.print(" ");
		Serial.print(ctrl_e);
		Serial.print(" ");
		Serial.print(p);
		Serial.print(" ");
		Serial.print(i);
		Serial.print(" ");
		Serial.print(d);
		Serial.print(" ");
		Serial.print(ctrl_e_sat);
		Serial.print(" ");
		Serial.println(ctrl_mapped_ref);
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
	/*	if(delay_time > 0)
			Serial.println("OK");
		else
			Serial.println("NOT OK");*/
	}
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
  pinMode(analogInPin,INPUT);
  pinMode(analogOutPin,OUTPUT);
}

void loop() {
  main_switch();
}

