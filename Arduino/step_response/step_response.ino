
#define PIN_LED 9
#define PIN_LDR A0

unsigned long time;
int ldr;

void setup() {
  pinMode(PIN_LED, OUTPUT);

  // initialize serial communications at 9600 bps:
  Serial.begin(115200);
  Serial.println("\n\ntime,\tLDR,\tLED");
}


void loop() {

	int time_ctrl;

  analogWrite(PIN_LED, 0);
  time_ctrl = millis();
  while (millis() < time_ctrl + 20){
    ldr = analogRead(PIN_LDR);
  	time = micros();
    Serial.print(time);
    Serial.print(",\t");
    Serial.print(ldr);
    Serial.print(",\t");
    Serial.println("0");
    delayMicroseconds(100);
  }

  analogWrite(PIN_LED, 128);
  time_ctrl = millis();
  while (millis() < time_ctrl + 300){
    ldr = analogRead(PIN_LDR);
  	time = micros();
    Serial.print(time);
    Serial.print(",\t");
    Serial.print(ldr);
    Serial.print(",\t");
    Serial.println("128");
    delayMicroseconds(100);
  }

  analogWrite(PIN_LED, 148);
  time_ctrl = millis();
  while (millis() < time_ctrl + 120){
    ldr = analogRead(PIN_LDR);
  	time = micros();
    Serial.print(time);
    Serial.print(",\t");
    Serial.print(ldr);
    Serial.print(",\t");
    Serial.println("148");
    delayMicroseconds(100);
  }

	delay(1000000);
}
