#include <Wire.h>
#include "PinChangeInterrupt.h"
#define PM10_IN_PIN 8   //input for PM10 signal, P2
#define PM2_IN_PIN 7  //input for PM2 signal, P1
#define PM10_FLAG 1
#define PM2_FLAG 2

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
  SWM_PM_SETUP();
  attachPCINT(digitalPinToPCINT(PM10_IN_PIN), calcPM10, CHANGE);
  attachPCINT(digitalPinToPCINT(PM2_IN_PIN), calcPM2, CHANGE);

  Serial.begin(115200);
  Serial.print("Dust Sensor sample rate of ");  Serial.print(samplerate / 1000); Serial.print(" sec, with rolling average over "); Serial.print(samplerate / 1000 * NoOfSamples); Serial.println(" sec.");
  Wire.begin ();
  TWBR = 152;  // 50 kHz 
}

void loop()
{
  if (millis() >= (samplerate + sampletime))
  {
    CalculateDustValue();
    Serial.print("PM2.5: "); Serial.print (PM2_Value); Serial.print(" "); Serial.write(181); Serial.print("g/m3"); Serial.print("\t");
    Serial.print("PM10:  "); Serial.print (PM10_Value); Serial.print(" "); Serial.write(181); Serial.print("g/m3  ");
   }
}

void SWM_PM_SETUP() {
  pinMode(PM10_IN_PIN, INPUT);
  pinMode(PM2_IN_PIN, INPUT);
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

  /* converts LP outputs to values, calculate % LPO first, then converet to µg/m3 assuming conversion is linear
              output (µS)                           concentration change (250 or 600)
     -----------------------------------    x 100 x ---------------------------------  + offset (0 or 250)
     sample rate (mS) x 1000 x NoOfSamples               percentage change (3 0r 7)
  */
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

  if (PM2_Value <= 12 && PM10_Value <= 54)
  {
    Serial.println("Green ");
  }
  else if (PM2_Value <= 35 && PM10_Value <= 154)
  {
    Serial.println("Yellow");
  }
  else if (PM2_Value <= 55 && PM10_Value <= 254)
  {
    Serial.println("Orange");
  }
  else if (PM2_Value <= 150 && PM10_Value <= 354)
  {
    Serial.println("Red");
  }
  else if (PM2_Value <= 250 && PM10_Value <= 424)
  {
    Serial.println("Purple");
  }
  else {
    Serial.println("Maroon");
  }
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
