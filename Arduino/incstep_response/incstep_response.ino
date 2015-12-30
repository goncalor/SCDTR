#define PIN_LED 5
#define PIN_LDR A0

unsigned long time;
int ldr;

void setup() {
  pinMode(PIN_LED, OUTPUT);

  Serial.begin(115200);
  Serial.println("time,\tLDR,\tLED");

  analogWrite(PIN_LED, 0);
  delay(1000);	// allow things to stabilize
}


void loop() {

  unsigned long time_ctrl;
  int pwm_val = 0;

  // note that each line of data will take about 1.25 ms to write, assuming 18
  // bytes per line and the baud rate of 115200.
  // also note that each analogRead() takes about 100 us to complete.
  for(pwm_val=0; pwm_val<=255; pwm_val+=10)
  {
    analogWrite(PIN_LED, pwm_val);
    time_ctrl = millis();
    while (millis() < time_ctrl + 200){
      ldr = analogRead(PIN_LDR);
      time = micros();
      Serial.print(time);
      Serial.print(",\t");
      Serial.print(ldr);
      Serial.print(",\t");
      Serial.println(pwm_val);
    }
  }

  // an analogRead() takes about 100 us to complete
  //time_ctrl = micros();
  //ldr = analogRead(PIN_LDR);
  //Serial.println(micros() - time_ctrl);
  //Serial.println(ldr);

  delay(1000000);
}
