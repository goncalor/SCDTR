
// the pins used:
const int analogInPin = A0;  // Analog input pin that the potentiometer is attached to
const int OutPin = 9; // Analog output pin that the LED is attached to

int sensorValue = 0;        // value read from the pot
int outputValue = 0;        // value output to the PWM (analog out)

void setup() {

  pinMode(OutPin,OUTPUT);

  // initialize serial communications at 9600 bps:
  Serial.begin(9600);
  Serial.println("LED, \t LDR");
}


void loop() {

  analogWrite(OutPin, 255);
  // read the analog in value:
  sensorValue = analogRead(analogInPin);

  Serial.print("HIGH,\t");
  Serial.println(sensorValue);

  delay(500);

  digitalWrite(OutPin,LOW);
  // read the analog in value:
  sensorValue = analogRead(analogInPin);
  // map it to the range of the analog out:

  Serial.print("LOW,\t");
  Serial.println(sensorValue);


  delay(500);
}
