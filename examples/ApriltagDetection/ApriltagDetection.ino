#include <SmartSensor.h>

// HardwareSerial pins
#define rxPin 0
#define txPin 1

// SoftwareSerial pins
//#define rxPin A5
//#define txPin A4

SmartSensor ss(rxPin, txPin);

float tag_size = 8; //  (cm) tag length

void setup()
{
  Serial.begin(9600);
	ss.begin();
  //ss.enableUVC(true);
}


void loop()
{
	if (ss.isDetected()) {
		if (ss.getFuncID() == ss.FUNC_APRILTAG) {
      float px=0, py=0, pz=0;
      int rx=0, ry=0, rz=0;
      int cx=0, cy=0;
      ss.getApriltagInfo(&px, &py, &pz, &rx, &ry, &rz, &cx, &cy);
      int dist = sqrt(px*px + py*py + pz*pz) * tag_size;
      Serial.print("(x,y,z)=(");
      Serial.print(px);
      Serial.print(", ");
      Serial.print(py);
      Serial.print(", ");
      Serial.print(pz);
      Serial.print(")  dist=");
      Serial.print(dist);
      Serial.println("cm");
      Serial.print("(pitch,roll,yaw)=(");
      Serial.print(rx);
      Serial.print(", ");
      Serial.print(ry);
      Serial.print(", ");
      Serial.print(rz);
      Serial.println(")degree");
      
      Serial.print("cx=");
      Serial.print(cx);
      Serial.print(" cy=");
      Serial.println(cy);
      Serial.println("");
      
		}
	}
 
	//delay(20);

}
