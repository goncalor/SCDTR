
#define GAIN_K 1

#define PIN_LED 9
#define PIN_LDR A0


int ctrl_u = 0;		   // value output to the PWM (analog out)
int ctrl_e = 0;		   // error value
int ctrl_y = 0;		   // value at the LDR voltage divider
int ctrl_ref = 500;


void setup() {
  pinMode(PIN_LED, OUTPUT);

  // initialize serial communications at 9600 bps:
  Serial.begin(9600);
  Serial.println("ctrl_u,\tctrl_y,\tctrl_e");
}


void loop() {

	ctrl_y = analogRead(PIN_LDR);

	ctrl_e = ctrl_ref - ctrl_y;
	ctrl_u = map(GAIN_K*ctrl_e, 0, 1023, 0, 255);
	//ctrl_u = GAIN_K*ctrl_e;

	constrain(ctrl_u, 0, 255);

	//	if(ctrl_u < 0)
	//		ctrl_u = 0;
	//	else if(ctrl_u > 255)
	//		ctrl_u = 255;

	analogWrite(PIN_LED, ctrl_u);

	Serial.print(ctrl_u);
	Serial.print(",\t ");
	Serial.print(ctrl_y);
	Serial.print(",\t ");
	Serial.println(ctrl_e);

	delayMicroseconds(10);

}
