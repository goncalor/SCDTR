
const int analogInPin = A0;  // Analog input pin that the potentiometer is attached to
const int OutPin = 5; // Analog output pin that the LED is attached to

int sensorValue = 0;        // value read from the pot
int pwm_val = 0;
int inc = 1;


void setup() {

  pinMode(OutPin,OUTPUT);

  // initialize serial communications at 9600 bps:
  Serial.begin(9600);
  Serial.println("LED, \t LDR");

  analogWrite(OutPin, pwm_val);
  delay(1000);    // wait for things to settle before starting measurements
}


void loop() {

  analogWrite(OutPin, pwm_val);
  // read the analog in value:
  sensorValue = analogRead(analogInPin);

  Serial.print(pwm_val);
  Serial.print(",\t ");
  Serial.println(sensorValue);

  pwm_val += inc;

  if(pwm_val == 255)
    inc = -1;
  else if(pwm_val == 0)
    inc = 1;

  delay(400);
}
