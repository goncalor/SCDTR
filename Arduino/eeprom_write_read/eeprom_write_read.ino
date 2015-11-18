/*
 * Read and write values into the first byte of the EEPROM
 * 
 */

#include <EEPROM.h>

#define BAUDRATE 115200
#define ADDRESS  0

/** the current address in the EEPROM (i.e. which byte we're going to write to next) **/
int addr = 0; 

void setup(){

    Serial.begin(BAUDRATE);
    Serial.print("baud rate: ");
    Serial.println(BAUDRATE);
}

void loop()
{
    char val;
    int intread;

    Serial.print("value read: ");
    Serial.println(EEPROM.read(ADDRESS));

    while(!Serial.available())
        delay(100);

    intread = Serial.parseInt();

    EEPROM.write(ADDRESS, intread);
}
