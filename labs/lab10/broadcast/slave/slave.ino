// Written by Nick Gammon
// February 2011
// http://gammon.com.au/i2c


#include <Wire.h>

const byte MY_ADDRESS = 42;   // me
const byte LED = 13;          // LED is on pin 13

byte ledVal = 0;

void receiveEvent (int howMany){
  // we are expecting 2 bytes, so check we got them
  if (howMany == 2){
    int result;
    result = Wire.read (); //store most significant byte
    result <<= 8; // shift it to the left
    result |= Wire.read (); // read least significant byte

    // do something with result here ...
    // for example, flash the LED
    digitalWrite (LED, ledVal ^= 1);   // flash the LED

  }  // end if 2 bytes were sent to us

  // throw away any garbage
  while (Wire.available () > 0)
  Wire.read ();

}  // end of receiveEvent


void setup (){
  Wire.begin (MY_ADDRESS);  // initialize hardware registers etc.

  TWAR = (MY_ADDRESS << 1) | 1;  // enable broadcasts to be received

  Wire.onReceive( receiveEvent );  // set up receive handler
  pinMode(LED, OUTPUT);          // for debugging, allow LED to be flashed
}  // end of setup


void loop (){
}  // end of loop
