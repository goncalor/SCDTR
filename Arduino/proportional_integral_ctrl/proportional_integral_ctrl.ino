
#define GAIN_K 2
#define INTEGRAL_TIME 0.001

#define PIN_LED 9
#define PIN_LDR A0

#define SAMPLE_TIME 250

int ctrl_u = 0;		   // value output to the PWM (analog out)
int ctrl_e = 0;		   // error value
int ctrl_y = 0;		   // value at the LDR voltage divider
int ctrl_ref = 500;
int ctrl_ui = 0;
int ctrl_ui_before = 0;

unsigned long start_time;
unsigned long end_time;
unsigned long write_time;
long delay_time;

void setup() {
  pinMode(PIN_LED, OUTPUT);

  // initialize serial communications at 9600 bps:
  Serial.begin(9600);
  Serial.println("time,\tctrl_u,\tctrl_y,\tctrl_e,\t ctrl_refs");
}


void loop() {

  start_time = micros();
	ctrl_y = analogRead(PIN_LDR);
	ctrl_e = ctrl_ref - ctrl_y;
	/* Proportional
  ctrl_u = GAIN_K*ctrl_e;

  */
  /*Proportinal-Integral*/
  ctrl_ui = ctrl_ui_before + SAMPLE_TIME/INTEGRAL_TIME * ctrl_e;
  ctrl_u = GAIN_K * ctrl_e + GAIN_K*INTEGRAL_TIME * (ctrl_ui + ctrl_e);

  ctrl_u = map(ctrl_u, 0, 1023, 0, 255); // Doesn't constrain to within range
	ctrl_u = constrain(ctrl_u, 0, 255);
  //	if(ctrl_u < 0)
  //		ctrl_u = 0;
  //	else if(ctrl_u > 255)
  //		ctrl_u = 255;

  write_time = micros();
  end_time = micros();
  ctrl_ui_before = ctrl_ui;





  delay_time = SAMPLE_TIME - (end_time - start_time);
  delayMicroseconds(delay_time);
	analogWrite(PIN_LED, ctrl_u);

  Serial.print(write_time);

  Serial.print(",\t");
	Serial.print(ctrl_u);
	Serial.print(",\t ");
	Serial.print(ctrl_y);
	Serial.print(",\t ");
	Serial.print(ctrl_e);
  Serial.print(",\t");
  Serial.print(ctrl_ref);
  Serial.print(",\t");
  if(delay_time > 0)
    Serial.println("OK");
  else
    Serial.println("NOT OK");



}
