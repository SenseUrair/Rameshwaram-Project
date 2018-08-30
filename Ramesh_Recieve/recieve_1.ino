#include <SPI.h>
#include <DMD2.h>
#include <fonts/Arial14.h>
#include <fonts/SystemFont5x7.h>

const int WIDTH = 4;
const uint8_t *FONT = Arial14;

SoftDMD dmd(WIDTH,1);  // DMD controls the entire display
DMD_TextBox box(dmd);

char chr[5];
char inData[70],aChar;
byte index=0;
const char *next;
int started(void);
int ended(void);

void setup(void)
{
 dmd.setBrightness(155);
 dmd.selectFont(FONT);
 dmd.begin();
 Serial.begin(9600); 
}
 
void loop(void)
{ 
  if(Serial.available() > 0)
  {
    aChar = Serial.read();  
    if(aChar == '<')
      started();     
    memset(inData,0,sizeof(inData));
  }
}

int started(void)
{
  for(int i =0; i<46; i++)
  {
    if(Serial.available() > 0)
    {
      inData[i] = Serial.read(); 
      box.print(inData[i]);
      Serial.print(inData[i]);
      if(inData[i] == '>')
        break; 
    }
  }
  Serial.println("Communication over");
  Serial.end();
  Serial.begin(9600);
}

int ended(void)
{
  next = inData;
  for(int i =0; i<46; i++)
  {
/*    if(next[i] == '>')
    {
      //Serial.println("trans over");
      Serial.end();
      Serial.begin(9600);
      break; 
    }*/   
//      Serial.print(*next);
      box.print(*next);
      next++;
  }
  //delay(500);
}
