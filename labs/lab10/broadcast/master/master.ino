// Written by Nick Gammon
// February 2011
// http://gammon.com.au/i2c

#include <Wire.h>

const byte MY_ADDRESS = 25; // me


void setup (){
  Wire.begin (MY_ADDRESS); // initialize hardware registers etc.
  Serial.begin(9600);
}  // end of setup


void loop(){
  unsigned int value = 1234; // ie. 0x04 0xD2

  Wire.beginTransmission (0); // broadcast to all
  Wire.write (highByte (value));
  Wire.write (lowByte (value));
  byte err = Wire.endTransmission(); // non-zero means error

  Serial.print("err=");
  Serial.println(err);
  delay (100); // wait 0.1 seconds
} // end of loop
