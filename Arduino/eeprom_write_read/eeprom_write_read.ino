/*
 * Read and write values into the first byte of the EEPROM
 * 
 */

#include <EEPROM.h>

#define BAUDRATE 115200
#define ADDRESS  0

void setup(){

    Serial.begin(BAUDRATE);
    Serial.print("baud rate: ");
    Serial.println(BAUDRATE);

    delay(1000);
}

void loop()
{
    byte val;

    delay(1000);

    val = EEPROM.read(ADDRESS);
    Serial.print("value read: ");
    Serial.println(val);

    while(!Serial.available())
        delay(100);

    val = Serial.parseInt();

    if(val!=0)
    {
        Serial.print("val: ");
        Serial.println(val);
        EEPROM.write((int) ADDRESS, val);
    }
    else
    {
        Serial.println("Ia escrever 0... Mas nao escrevi :)");
    }

}
