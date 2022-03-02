/*
 * Copyright 2022 VIA Technologies, Inc. All Rights Reserved.
 *
 * This PROPRIETARY SOFTWARE is the property of VIA Technologies, Inc.
 * and may contain trade secrets and/or other confidential information of
 * VIA Technologies, Inc. This file shall not be disclosed to any third
 * party, in whole or in part, without prior written consent of VIA.
 *
 * THIS PROPRIETARY SOFTWARE AND ANY RELATED DOCUMENTATION ARE PROVIDED AS IS,
 * WITH ALL FAULTS, AND WITHOUT WARRANTY OF ANY KIND EITHER EXPRESS OR IMPLIED,
 * AND VIA TECHNOLOGIES, INC. DISCLAIMS ALL EXPRESS OR IMPLIED
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET
 * ENJOYMENT OR NON-INFRINGEMENT.
*/

#include <Pixetto.h>
#include <Servo.h>

const int rxPin = 0;
const int txPin = 0;
const int servoPin = 9;
const int motorPin1 = 3;
const int motorPin2 = 5;

// TUNE THESE
float Kp = 1.0;
float Kd = 0.0;

Pixetto ss(rxPin, txPin);
Servo steering_servo;
int x, err, prev_err, cmd;

void setup(){
  ss.begin();
  ss.enableFunc(Pixetto::FUNC_LANES_DETECTION);

  steering_servo.attach(servoPin);

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
    digitalWrite(LED_BUILTIN, HIGH);
    analogWrite(motorPin1, 45);
    analogWrite(motorPin2, 45);

    x = ss.getPosX();
    err = x - 50;
    int d_err = err-prev_err;
    cmd = (int)(90 + Kp * err + Kd * d_err);
    steering_servo.write(cmd);
    prev_err = err;
  }
  // delay(10);
}
