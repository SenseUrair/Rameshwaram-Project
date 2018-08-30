#include <SPI.h>
#include <Wire.h>
#include "PinChangeInterrupt.h"
//#include<SoftwareSerial.h>
//SoftwareSerial gsm(9,10);

#define PM10_IN_PIN 8   //input for PM10 signal, P2
#define PM2_IN_PIN 7  //input for PM2 signal, P1
#define PM10_FLAG 1
#define PM2_FLAG 2

#define ADDR_6713 0x15 

float O3_Conc=0.0;
int data [4];
int CO2ppmValue;

float p25,p10,next;
float WE,WE_SO,AE,Final_SO,V1,V2,V1_SO,High_sensitive;
int16_t adc3,adc4,adc5,adc6,dust,noise;
float Voffset=0.0;
int percentage;
float volts,adc1,adc2;
int co2Value;
int gsm_arr[4];
volatile uint8_t bUpdateFlagsShared;

unsigned long samplerate = 5000;
unsigned long sampletime;
int SampleCount;
int NoOfSamples = 12;  //maximum 14
static long  PM2_Value;
static long  PM10_Value;
String AQIColour;

volatile uint16_t unPM10_InShared;
volatile uint16_t unPM2_InShared;

uint32_t ulPM10_Start;
uint32_t ulPM2_Start;

void setup()
{
  Wire.begin ();
  //gsm.begin(9600);
  Serial.begin(9600);

  pinMode(A0,INPUT);
  pinMode(A1,INPUT);
  pinMode(PM10_IN_PIN, INPUT);
  pinMode(PM2_IN_PIN, INPUT);
  attachPCINT(digitalPinToPCINT(PM10_IN_PIN), calcPM10, CHANGE);
  attachPCINT(digitalPinToPCINT(PM2_IN_PIN), calcPM2, CHANGE);
  TWBR = 152;
}

void loop()
{
  if (millis() >= (samplerate + sampletime))
    CalculateDustValue();
    
   adc1 = analogRead(A0);//CO
   adc2 = analogRead(A1);//CO

 
// WE = adc1-342;
// AE = adc2-341; 
 V1 = (adc1 * 5)/1023;
 V2 = (adc2 * 5)/1023;
 next = V1-(1.2)*(93/-12)*V2;
 next = (next/4.0);
   
 co2Value =readC02();
 
 Serial.print("<");
 Serial.print("PM2.5="); 
 Serial.print (PM2_Value); 
 gsm_arr[0] = PM2_Value;
 Serial.print("µg/m3"); 
 Serial.print(" "); 
 Serial.print("PM10="); 
 Serial.print (PM10_Value); 
 Serial.print("µg/m3");
 gsm_arr[1] = PM10_Value;
 Serial.print(" ");
 Serial.print("CO=");
 if(next <= .10)
  Serial.print("0.10");
 else
   Serial.print(next);
 Serial.print("ppm"); 
 Serial.print(" "); 
 Serial.print("CO2=");
 Serial.print(co2Value);
 Serial.print("ppm");  
 gsm_arr[3] = co2Value;
 Serial.print("\n");
 Serial.print(">");   
 //Send2web();
}

void CalculateDustValue() {
  static uint16_t unPM10_In;
  static uint16_t unPM2_In;
  static uint16_t unPM10_Time;
  static uint16_t unPM2_Time;
  // local copy of update flags
  static uint8_t bUpdateFlags;
  static long    PM2_Output[15];
  static long    PM10_Output[15];

  if (bUpdateFlagsShared)
  {
    noInterrupts();
    bUpdateFlags = bUpdateFlagsShared;

    if (bUpdateFlags & PM10_FLAG)
    {
      unPM10_In = unPM10_InShared;
      unPM10_Time = (unPM10_Time + unPM10_In);
    }
    if (bUpdateFlags & PM2_FLAG)
    {
      unPM2_In = unPM2_InShared;
      unPM2_Time = (unPM2_Time + unPM2_In);
    }
    bUpdateFlagsShared = 0;
    interrupts(); 
  }
  sampletime = millis();  //resets timer before printing output
  PM2_Output[SampleCount] = unPM2_Time ;
  PM10_Output[SampleCount] = unPM10_Time ;
  unPM2_Time = 0;
  unPM10_Time = 0;
  
  PM2_Output[0] = PM2_Output[1] + PM2_Output[2] + PM2_Output[3] + PM2_Output[4] + PM2_Output[5] + PM2_Output[6] + PM2_Output[7] + PM2_Output[8]+ PM2_Output[9]+ PM2_Output[10]+ PM2_Output[11]+ PM2_Output[12];
  PM10_Output[0] = PM10_Output[1] + PM10_Output[2] + PM10_Output[3] + PM10_Output[4] + PM10_Output[5] + PM10_Output[6] + PM10_Output[7] + PM10_Output[8] + PM10_Output[9] + PM10_Output[10] + PM10_Output[11] + PM10_Output[12];

  if (PM2_Output[0] / (samplerate * NoOfSamples * 10 ) >= 3 || PM10_Output[0] / (samplerate * NoOfSamples * 10 ) >= 3);
  {
    PM2_Value = round((float)PM2_Output[0] / (samplerate * NoOfSamples * 10 ) * 600 / 7 + 250);
    PM10_Value = round((float)PM10_Output[0] / (samplerate * NoOfSamples * 10 ) * 600 / 7 + 250);
  }
  {
    PM2_Value = round((float)PM2_Output[0] / (samplerate * NoOfSamples * 10 ) * 250 / 3);
    PM10_Value = round((float)PM10_Output[0] / (samplerate * NoOfSamples * 10 ) * 250 / 3);
  }
  bUpdateFlags = 0;  //reset flags and variables

  if (SampleCount >= NoOfSamples)
    SampleCount = 1;
  else
    SampleCount++;
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
 CO2ppmValue = ((data[2] * 0xFF ) + data[3]);
 return(CO2ppmValue);
}
void calcPM10()
{
  if (digitalRead(PM10_IN_PIN) == LOW)
  {
    ulPM10_Start = micros();
  }
  else
  {
    unPM10_InShared = (uint16_t)(micros() - ulPM10_Start);
    bUpdateFlagsShared |= PM10_FLAG;
  }
}

void calcPM2()
{
  if (digitalRead(PM2_IN_PIN) == LOW)
  {
    ulPM2_Start = micros();
  }
  else
  {
    unPM2_InShared = (uint16_t)(micros() - ulPM2_Start);
    bUpdateFlagsShared |= PM2_FLAG;
  }
}
