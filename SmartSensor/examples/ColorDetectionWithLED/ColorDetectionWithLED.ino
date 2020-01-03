#include <SmartSensor.h>

// SoftwareSerial pins
#define rxPin 12
#define txPin 10

// HardwareSerial pins
//#define rxPin 0
//#define txPin 1

SmartSensor ss(rxPin, txPin);

void setup()
{
	ss.begin();

	pinMode(13, OUTPUT);
	pinMode(11, OUTPUT);
}


void loop()
{
	if (ss.isDetected()) {
		if (ss.getFuncID() == ss.FUNC_COLOR_DETECTION) {
			if (ss.getTypeID() == ss.COLOR_RED){
				digitalWrite(13, HIGH);
				digitalWrite(11, LOW);
			} else if (ss.getTypeID() == ss.COLOR_BLUE){
				digitalWrite(11, HIGH);
				digitalWrite(13, LOW);
			}
		}
	}
	delay(20);

}
