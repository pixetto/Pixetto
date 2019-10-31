#include <Wire.h>
#include <AutoCar.h>
#include <SmartSensor.h>

#define rxPin 10
#define txPin 11

SoftwareSerial swSer = SoftwareSerial(rxPin, txPin);
SmartSensor ss(&swSer);

AutoCar car;

int nFuncID = 0;
int nTypeID = 0;
int nPosX = 0;
int nPosY = 0;
int nPosW = 0;
int nPosH = 0;
int nPosCenter = 0;

int nOldX = 0;
int nOldY = 0;
int nOldW = 0;
int nOldH = 0;


void forward()
{
	String s = "";
	if(ss.isDetected())
	{
		nFuncID = ss.getFuncID();
		nTypeID = ss.getTypeID();
		nPosX = ss.getPosX();
		nPosY = ss.getPosY();
		nPosH = ss.getH();
		nPosW = ss.getW();

		nPosCenter = nPosX + nPosW/2;
		if (nPosCenter > 500 + 125)
		{
		    car.turnRight(30);
		    Serial.println("Right");
		}
		else if (nPosCenter < 500 - 125)
		{
		    car.turnLeft(30);
		    Serial.println("Left");
  		}
  		else
  		    car.goStraight();

		if (nPosW > 350)
		{
		    car.drive(0);
		}
        else if (nOldW > nPosW)
        {
			if ((nOldW - nPosW) > 10)
			{
				car.drive(55);
				Serial.println("Far");
			}
		}
		else
		{
			if ((nPosW - nOldW) > 10)
			{
				car.drive(35);
				Serial.println("Close");
			}
		}
		nOldW = nPosW;
   }
   else
   {
       Serial.println("NONE");
   }

}

void setup()
{
  Wire.begin();
  Serial.begin(9600);

  ss.begin();

  car.init();
  car.configMotor(0, 3, 2);
  car.configMotor(1, 5, 4);
  car.configServo(9);
  car.initMLPModule();
  car.initEdgeDetectModule();
  car.initHoughModule();
  car.configDriverless(0);
  car.drive(20);
}


void loop()
{
  forward();
  delay(100);
}
