
// the pins used:
const int analogInPin = A0;  // Analog input pin that the potentiometer is attached to
const int OutPin = 9; // Analog output pin that the LED is attached to

int sensorValue = 0;        // value read from the pot
int outputValue = 0;        // value output to the PWM (analog out)


int pwm_val = 0;
int inc = 1;


void setup() {

  pinMode(OutPin,OUTPUT);

  // initialize serial communications at 9600 bps:
  Serial.begin(9600);
  Serial.println("LED, \t LDR");


}


void loop() {

  analogWrite(OutPin, pwm_val);
  // read the analog in value:
  sensorValue = analogRead(analogInPin);

  pwm_val += inc;

  if(pwm_val == 255)
    inc = -1;
  else if(pwm_val == 0)
    inc = 1;

  Serial.print(pwm_val);
  Serial.print(",\t ");
  Serial.println(sensorValue);

  delay(400);

}
