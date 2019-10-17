#include <SmartSensor.h>

#define rxPin 12
#define txPin 10

SoftwareSerial swSer = SoftwareSerial(rxPin, txPin);
SmartSensor ss(&swSer);

void setup()
{
    Serial.begin(9600);
	ss.begin();
	
	pinMode(6, OUTPUT);
	pinMode(9, OUTPUT);
	pinMode(11, OUTPUT);
	pinMode(13, OUTPUT);

	Serial.println("setup()");
}

int nFuncID = 0;
int nTypeID = 0;
int nPosX = 0;
int nPosY = 0;
int nPosW = 0;
int nPosH = 0;

int nOldX = 0;
int nOldY = 0;
int nOldW = 0;
int nOldH = 0;

void loop()
{
  String s = "";
  if (ss.isDetected())
  {
       nFuncID = ss.getFuncID();
       nTypeID = ss.getTypeID();
       nPosX = ss.getPosX();
       nPosY = ss.getPosY();
       nPosH = ss.getH();
       nPosW = ss.getW();

       if (nOldX > nPosX && (nOldX - nPosX) > 20)
       {
          digitalWrite(13,HIGH);
          delay(50);
          digitalWrite(13,LOW);
          Serial.println("Left");
       }
       else if (nPosX > nOldX && (nPosX - nOldX) > 20)
       {
          digitalWrite(11,HIGH);
          delay(50);
          digitalWrite(11,LOW);
		  Serial.println("Right");
       }
       nOldX = nPosX;
       
       if (nOldW > nPosW && (nOldW - nPosW) > 10)
       {
          digitalWrite(6,HIGH);
          delay(50);
          digitalWrite(6,LOW);
          Serial.println("Far");
       }
       else if (nPosW > nOldW && (nPosW - nOldW) > 10)
       {
          digitalWrite(9,HIGH);
          delay(50);
          digitalWrite(9,LOW);
		  Serial.println("Close");
       }
       nOldW = nPosW;
   }
   else
   {
       Serial.println("NONE");
   }

   delay(100);
 }


