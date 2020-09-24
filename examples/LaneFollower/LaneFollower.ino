#include <SmartSensor.h>
#include <Servo.h>

// TUNE THESE
float Kp = 1.0;
float Kd = 0.0;

SmartSensor ss(0,1);
Servo steering_servo;
int x, err, prev_err, cmd;

void setup(){
  ss.begin();
  steering_servo.attach(9);

  x = 0;
  err = 0;
  prev_err = 0;  
  cmd = 0;

  steering_servo.write(90);
  pinMode(13, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(5, OUTPUT);
}

void loop(){
  if (ss.isDetected()) {
      digitalWrite(13, HIGH);
      analogWrite(3, 45);
      analogWrite(5, 45);
      
      x = ss.getPosX();
      err = x - 50;
      int d_err = err-prev_err;
      cmd = (int)(90 + Kp * err + Kd * d_err);
      steering_servo.write(cmd);
      prev_err = err;
  }
  delay(10);
}
