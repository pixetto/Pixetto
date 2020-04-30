#include <SmartSensor.h>
#include <Servo.h>

#define IMG_CENTER_X  50
#define IMG_CENTER_Y  50
#define MIN_DEG_X   20
#define MAX_DEG_X   160
#define MIN_DEG_Y   20
#define MAX_DEG_Y   160
#define DEG_CENTER    90

#define servoXPin 9
#define servoYPin 10

// SoftwareSerial pins
#define rxPin A2
#define txPin A3

// HardwareSerial pins
//#define rxPin 0
//#define txPin 1


SmartSensor ss(rxPin, txPin);

Servo servo_x;
Servo servo_y;


int nFuncID = 0;
int nTypeID = 0;
int nCenterX = 0;
int nCenterY = 0;
int diffX=0, diffY=0;
int moveX=0, moveY=0;
int a = 0;
int b = 0;


void setup()
{
	pinMode(rxPin, INPUT);
	pinMode(txPin, OUTPUT);
	pinMode(servoXPin, OUTPUT);
	pinMode(servoYPin, OUTPUT);

	ss.begin();
  //ss.enableUVC(true);
	servo_x.attach(servoXPin);
	servo_y.attach(servoYPin);
	servo_x.write(DEG_CENTER);
	servo_y.write(DEG_CENTER);
}

void loop()
{
	String s = "";
	if(ss.isDetected())
	{
		nFuncID = ss.getFuncID();
		nTypeID = ss.getTypeID();

    nCenterX = ss.getPosX() + ss.getWidth()/2;

		if (nCenterX > IMG_CENTER_X + 6)
		{
			diffX = nCenterX - IMG_CENTER_X;
			moveX = map(diffX, 6,IMG_CENTER_X,0,6);

			b = servo_y.read();
			if (b < DEG_CENTER)
			{
				a = servo_x.read() - moveX;
				if (a < MIN_DEG_X) a = MIN_DEG_X;
			}
			else
			{
				a = servo_x.read() + moveX;
				if (a > MAX_DEG_X) a = MAX_DEG_X;
			}
			servo_x.write(a);
		}
		else if (nCenterX < IMG_CENTER_X - 6)
		{
			diffX = IMG_CENTER_X - nCenterX;
			moveX = map(diffX, 6,IMG_CENTER_X,0,6);

			b = servo_y.read();
			if (b < DEG_CENTER)
			{
				a = servo_x.read() + moveX;
				if (a > MAX_DEG_X) a = MAX_DEG_X;
			}
			else
			{
				a = servo_x.read() - moveX;
				if (a < MIN_DEG_X) a = MIN_DEG_X;
			}
			servo_x.write(a);
		}

		nCenterY = ss.getPosY() + ss.getHeight()/2;

		if (nCenterY > IMG_CENTER_Y + 8)
		{
			diffY = nCenterY - IMG_CENTER_Y;
			moveY = map(diffY, 8,IMG_CENTER_Y,0,4);
			b = servo_y.read() - moveY;
			if (b < MIN_DEG_Y) b = MIN_DEG_Y;
			servo_y.write(b);
		}
		else if (nCenterY < IMG_CENTER_Y - 8)
		{
			diffY = IMG_CENTER_Y - nCenterY;
			moveY = map(diffY, 8,IMG_CENTER_Y,0,4);
			b = servo_y.read() + moveY;
			if (b > MAX_DEG_Y) b = MAX_DEG_Y;
			servo_y.write(b);
		}
	}

	delay(20);
}
