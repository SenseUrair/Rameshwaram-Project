#include <SPI.h>
#include <DMD2.h>
#include <fonts/Arial14.h>
#include "DHT.h"
#include <Wire.h>
#include "RTClib.h"

#define DHTPIN 2     
#define DHTTYPE DHT22   
DHT dht(DHTPIN, DHTTYPE);

RTC_DS1307 RTC;

const int COUNTDOWN_FROM = 12;
int counter = COUNTDOWN_FROM;

SoftDMD dmd(4,1); 
DMD_TextBox box(dmd, 0, 2);

void setup() {
  Serial.begin(9600);
  dht.begin();
  dmd.setBrightness(95);
  dmd.selectFont(Arial14);
  dmd.begin();
  Wire.begin();
  RTC.begin();
}

void loop() {
  
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  
  //RTC.adjust(DateTime(__DATE__, __TIME__));
  DateTime now = RTC.now(); 
  Serial.print(now.day(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.year(), DEC);
  Serial.print(' ');
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.println();
  Serial.print(" Temperature=");
  Serial.print(t);
  Serial.print("c");
  Serial.print("   ");
  Serial.print(" Humidity=");
  Serial.print(h);
  Serial.print("%");
    
    box.print("   ");
    box.print("Date= ");
    box.print(now.day(), DEC);
    box.print('/');
    box.print(now.month(), DEC);
    box.print('/');
    box.print(now.year(), DEC);
    
    box.print("  ");
    box.print("Time= ");
    box.print(now.hour(), DEC);
    box.print(':');
    box.print(now.minute(), DEC);
    box.print(':');
    box.print(now.second(), DEC);
    box.print("   ");
    box.print("Temperature= ");
    box.print(t);
    box.print("c");
    box.print("   ");
    box.print("Humidity= ");
    box.print(h);
    box.print("%");
    delay(500);
}

