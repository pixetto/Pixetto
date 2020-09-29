#include <SmartSensor.h>

// HardwareSerial pins
#define rxPin 0
#define txPin 1

// SoftwareSerial pins
//#define rxPin A5
//#define txPin A4

SmartSensor ss(rxPin, txPin);

int led = 13;

void setup()
{
	ss.begin();
  //ss.enableUVC(true);
  
	pinMode(led, OUTPUT);
}


void loop()
{
	if (ss.isDetected()) {
		// Available voice commands are defined in SmartSensor.h.
		if (ss.getFuncID() == ss.FUNC_VOICE_COMMAND) {
			if (ss.getTypeID() == ss.VOICE_TurnOnLight){
				digitalWrite(led, HIGH);
			} else if (ss.getTypeID() == ss.VOICE_TurnOffLight){
				digitalWrite(led, LOW);
			}
		} 
	}
	//delay(20);
}
