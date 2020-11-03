/*
 * Copyright 2020 VIA Technologies, Inc. All Rights Reserved.
 *
 * This PROPRIETARY SOFTWARE is the property of WonderMedia Technologies, Inc.
 * and may contain trade secrets and/or other confidential information of
 * WonderMedia Technologies, Inc. This file shall not be disclosed to any third
 * party, in whole or in part, without prior written consent of WonderMedia.
 *
 * THIS PROPRIETARY SOFTWARE AND ANY RELATED DOCUMENTATION ARE PROVIDED AS IS,
 * WITH ALL FAULTS, AND WITHOUT WARRANTY OF ANY KIND EITHER EXPRESS OR IMPLIED,
 * AND WonderMedia TECHNOLOGIES, INC. DISCLAIMS ALL EXPRESS OR IMPLIED
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET
 * ENJOYMENT OR NON-INFRINGEMENT.
*/

#include <Pixetto.h>
#include <Servo.h>

// TUNE THESE
float Kp = 1.0;
float Kd = 0.0;

Pixetto ss(0,1);
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
