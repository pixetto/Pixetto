#include <Wire.h>
#include <ImageProcessing.h>

ImageProcessing ip;

void setup() {
  // put your setup code here, to run once:
  Wire.begin();
  Serial.begin(9600);
  ip.init();
}

void loop() {
  // put your main code here, to run repeatedly:
  if(ip.detect(MODE_SHAPE)) {
    int num = ip.getNumOfResult();
    Serial.print("num=");
    Serial.println(num);
    for(int i=0; i<num; i++) {
      int index = ip.getIndexOfResult(i);
      Serial.print("index=");
      Serial.println(index);
    }
  }
  Serial.println("-------------------------");

}
