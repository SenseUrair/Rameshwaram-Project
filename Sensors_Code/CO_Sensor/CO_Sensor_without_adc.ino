float WE,AE,Final_CO,V1,V2,High_sensitive;
float adc1,adc2;
float a,b,c,d,e,f;
void setup() 
{
  Serial.begin(9600);

  pinMode(A0,INPUT);
  pinMode(A1,INPUT);
}

void loop() 
{
  adc1 = analogRead(A0);
  adc2 = analogRead(A1);

  V1 = (adc1 * 5)/1023;
  V2 = (adc2 * 5)/1023;

  Final_CO = V1-(3.5)*V2;
  float next = V1-(1.2)*(93/-12)*V2;
  next = (next/4.0);
  Serial.println(next);
  
  High_sensitive = Final_CO/439;
  delay(1000);
}
