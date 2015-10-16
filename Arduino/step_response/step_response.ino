
#define PIN_LED 9
#define PIN_LDR A0

unsigned long time;
int ldr;

void setup() {
  pinMode(PIN_LED, OUTPUT);

  // initialize serial communications at 9600 bps:
  Serial.begin(115200);
  Serial.println("time,\tLDR,\tLED");
}


void loop() {

  // First put LED to ground;

  analogWrite(PIN_LED,128);
  int i =0;
  while (i<1000){
    ldr = analogRead(PIN_LDR);
  	time = micros();
    Serial.print(time);
    Serial.print(",\t");
    Serial.print(ldr);
    Serial.print(",\t");
    Serial.println("128");
    delayMicroseconds(1000);
    i++;
  }
  analogWrite(PIN_LED,138);
  while (1) {
    ldr = analogRead(PIN_LDR);
    time = micros();
    Serial.print(time);
    Serial.print(",\t");
    Serial.print(ldr);
    Serial.print(",\t");
    Serial.println("138");
  }



}
