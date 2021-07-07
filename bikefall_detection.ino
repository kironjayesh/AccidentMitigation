#include <GPRS_Shield_Arduino.h>
#include <SoftwareSerial.h>
#include <Wire.h>

#define PIN_TX    8    /* rx of Arduino (connect tx of gprs to this pin) */
#define PIN_RX    7   /* tx of Arduino (connect rx of gprs to this pin) */
#define BAUDRATE  9600
#define PHONE_NUMBER "9092330551"
const int MPU_addr=0x68;
int16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;
 
int minVal=265;
int maxVal=402;
int button = 10; 
int flag = 0;
double x;
double y;
double z;
GPRS Sim900_test(PIN_TX,PIN_RX,BAUDRATE);  
 
void setup(){ 
  while(!Sim900_test.init())  /* Sim card and signal check, also check if module connected */ 
  {
      delay(1000);
      Serial.println("SIM900 initialization error");
  }  
  Serial.println("SIM900 initialization success");
Wire.begin();
Wire.beginTransmission(MPU_addr);
Wire.write(0x6B);
Wire.write(0);
Wire.endTransmission(true);
Serial.begin(9600);
pinMode(9,OUTPUT);
pinMode(10, INPUT_PULLUP);
}
void loop(){

int push;
Wire.beginTransmission(MPU_addr);
Wire.write(0x3B);
Wire.endTransmission(false);
Wire.requestFrom(MPU_addr,14,true);
AcX=Wire.read()<<8|Wire.read();
AcY=Wire.read()<<8|Wire.read();
AcZ=Wire.read()<<8|Wire.read();
int xAng = map(AcX,minVal,maxVal,-90,90);
int yAng = map(AcY,minVal,maxVal,-90,90);
int zAng = map(AcZ,minVal,maxVal,-90,90);
 
x= RAD_TO_DEG * (atan2(-yAng, -zAng)+PI);
y= RAD_TO_DEG * (atan2(-xAng, -zAng)+PI);
z= RAD_TO_DEG * (atan2(-yAng, -xAng)+PI);
 
Serial.print("AngleX= ");
Serial.println(x);
 
Serial.print("AngleY= ");
Serial.println(y);
 
Serial.print("AngleZ= ");
Serial.println(z);
Serial.println("-----------------------------------------");
push = digitalRead(button);
Serial.println("Push Value");
Serial.println(push);
if((x>15 && x<345) || (y>30 && y<330) )
{
  flag = flag +1;
  Serial.println("Flag Value");
  Serial.println(flag);
}

if(flag>3 && push == 1)
{
  Serial.println("Bike is in a toppled position");
  digitalWrite(9,HIGH);
  delay(500);
  if(flag>5)
  { Serial.println("Calling to inform");
        Sim900_test.callUp(PHONE_NUMBER); /* Call */
        delay(25000);
        Sim900_test.hangup(); /* Hang up the call */
  }
}
else if(push == 0)
{
  flag = 0;
  digitalWrite(9,LOW);
}
delay(4000);
}
