
#define PIN_LED 9
#define PIN_LDR A0

unsigned long time;
int ldr;

void setup() {
  pinMode(PIN_LED, OUTPUT);

  // initialize serial communications at 9600 bps:
  Serial.begin(9600);
  Serial.println("time,\tLDR,\tLED");
}


void loop() {

  // First put LED to ground;

  digitalWrite(PIN_LED,LOW);
  ldr = analogRead(PIN_LDR);
	time = micros();
  Serial.print(time);
  Serial.print(",\t");
  Serial.print(ldr);
  Serial.print(",\t");
  Serial.println("0");
  delayMicroseconds(1000);
  ldr = analogRead(PIN_LDR);
	time = micros();
  Serial.print(time);
  Serial.print(",\t");
  Serial.print(ldr);
  Serial.print(",\t");
  Serial.println("0");
  delayMicroseconds(1000);
  ldr = analogRead(PIN_LDR);
	time = micros();
  Serial.print(time);
  Serial.print(",\t");
  Serial.print(ldr);
  Serial.print(",\t");
  Serial.println("0");
  delayMicroseconds(1000);
  ldr = analogRead(PIN_LDR);
	time = micros();
  Serial.print(time);
  Serial.print(",\t");
  Serial.print(ldr);
  Serial.print(",\t");
  Serial.println("0");
  delayMicroseconds(1000);
  ldr = analogRead(PIN_LDR);
	time = micros();
  Serial.print(time);
  Serial.print(",\t");
  Serial.print(ldr);
  Serial.print(",\t");
  Serial.println("0");
  delayMicroseconds(1000);
  digitalWrite(PIN_LED,HIGH);
  ldr = analogRead(PIN_LDR);
	time = micros();
  Serial.print(time);
  Serial.print(",\t");
  Serial.print(ldr);
  Serial.print(",\t");
  Serial.println("1024");

  while (1) {
    ldr = analogRead(PIN_LDR);
    time = micros();
    Serial.print(time);
    Serial.print(",\t");
    Serial.print(ldr);
    Serial.print(",\t");
    Serial.println("1024");
  }



}
