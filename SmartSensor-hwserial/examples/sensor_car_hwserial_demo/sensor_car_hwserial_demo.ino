#include <Wire.h>
#include <AutoCar.h>
#include <SmartSensor-hwserial.h>

SmartSensor ss;

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
		}
		else if (nPosCenter < 500 - 125)
		{
		    car.turnLeft(30);
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
			}
		}
		else
		{
			if ((nPosW - nOldW) > 10)
			{
				car.drive(35);
			}
		}
		nOldW = nPosW;
   }

}

void setup()
{
  Wire.begin();

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
