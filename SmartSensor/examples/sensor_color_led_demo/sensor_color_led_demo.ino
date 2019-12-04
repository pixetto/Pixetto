#include <Arduino.h>
#include <SmartSensor.h>

SoftwareSerial swSer = SoftwareSerial(12,10);
SmartSensor ss(&swSer);

void setup()
{
  ss.begin();

  pinMode(13, OUTPUT);
  pinMode(11, OUTPUT);
}


void loop()
{
  if (ss.isDetected()) {
  	if (ss.getTypeID() == 1) {
  		digitalWrite(13, HIGH);
  		digitalWrite(11, LOW);
  	} else if (ss.getTypeID() == 4) {
  		digitalWrite(11, HIGH);
  		digitalWrite(13, LOW);
  	}
  }
  delay(20);

}
