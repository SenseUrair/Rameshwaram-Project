#include <Wire.h>
#define ADDR_6713 0x15 
int data [4];
int CO2ppmValue;

void setup() {
 Wire.begin ();
 Serial.begin(9600);
}

int readC02()
{
 Wire.beginTransmission(ADDR_6713);
 Wire.write(0x04); Wire.write(0x13); Wire.write(0x8B);
 Wire.write(0x00); Wire.write(0x01);
 Wire.endTransmission();
 delay(2000);
 Wire.requestFrom(ADDR_6713, 4); 
 data[0] = Wire.read();
 data[1] = Wire.read();
 data[2] = Wire.read();
 data[3] = Wire.read();
 Serial.print("Func code: "); Serial.print(data[0],HEX);
 Serial.print(" byte count: "); Serial.println(data[1],HEX);
 Serial.print("MSB: 0x"); Serial.print(data[2],HEX);
Serial.print(" ");
 Serial.print("LSB: 0x"); Serial.print(data[3],HEX);
Serial.print(" ");
 CO2ppmValue = ((data[2] * 0xFF ) + data[3]);
 Serial.println(CO2ppmValue);
}
void loop() {
 int co2Value =readC02();
 {
 Serial.print("CO2 Value: ");
 
 }

 delay(2000);
} 
