#include <Wire.h>
#include <AutoCar.h>

AutoCar car;

void forward() {
  if (car.getRDistance() - car.getLDistance() > 50) {
    car.drive(50);
    car.turnRight(50);
  } else if (car.getLDistance() - car.getRDistance() > 50) {
    car.drive(50);
    car.turnLeft(50);
  } else {
    car.drive(45);
    car.goStraight();
  }
}

void setup()
{
  Wire.begin();
  Serial.begin(9600);
  car.init();
  car.configMotor(0, 3, 2);
  car.configMotor(1, 5, 4);
  car.configServo(9);
  car.initMLPModule();
  car.initEdgeDetectModule();
  car.initHoughModule();
  car.configDriverless(1);
  car.drive(55);
}


void loop()
{
  car.probe();
  if (car.getRDistance() >= 0 || car.getLDistance() >= 0) {
    forward();
  }
  if (car.signDetected() && car.getSignRatio() > 10) {
    switch (car.getSignType()) {
     case 1:
      car.brake();
      break;
     case 6:
      car.drive(80);
      break;
     case 7:
      car.drive(50);
      break;
     case 8:
      car.turnLeft(30);
      car.drive(30);
      delay(1 * 1000);
      break;
     case 9:
      car.turnRight(30);
      car.drive(30);
      delay(1 * 1000);
      break;
    }
  }
}
