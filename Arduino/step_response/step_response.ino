#define PIN_LED 5
#define PIN_LDR A0

unsigned long time;
int ldr;

void setup() {
  pinMode(PIN_LED, OUTPUT);

  Serial.begin(115200);
  Serial.println("\n\ntime,\tLDR,\tLED");

  analogWrite(PIN_LED, 0);
  delay(1000);	// allow things to stabilize
}


void loop() {

  unsigned long time_ctrl;

  // note that each line of data will take about 1.25 ms to write, assuming 18
  // bytes per line and the baud rate of 115200.
  // also note that each analogRead() takes about 100 us to complete.
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
  }

  analogWrite(PIN_LED, 128);
  time_ctrl = millis();
  while (millis() < time_ctrl + 400){
    ldr = analogRead(PIN_LDR);
    time = micros();
    Serial.print(time);
    Serial.print(",\t");
    Serial.print(ldr);
    Serial.print(",\t");
    Serial.println("128");
  }

  // an analogRead() takes about 100 us to complete
  //time_ctrl = micros();
  //ldr = analogRead(PIN_LDR);
  //Serial.println(micros() - time_ctrl);
  //Serial.println(ldr);

  delay(1000000);
}
