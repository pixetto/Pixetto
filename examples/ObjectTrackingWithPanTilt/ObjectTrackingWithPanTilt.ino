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
const int txPin = 1;

Pixetto ss(rxPin, txPin);

Servo servo_9;
Servo servo_10;

void setup()
{
  ss.begin();
  ss.enableFunc(Pixetto::FUNC_COLOR_DETECTION);

  servo_9.attach(9);
  servo_10.attach(10);

  servo_9.write(90);   /* counterclockwise,  0 - 180, right-to-left */
  servo_10.write(135); /* counterclockwise, 90 - 160, up-to-down */
}

void loop()
{
  if (ss.isDetected()) {
    int x = ss.getPosX() + ss.getWidth() / 2;
    int dx = x - 50;
    int y = ss.getPosY() + ss.getHeight() / 2;
    int dy = y - 50;

    if (dx < ss.getWidth()/-2) {
      if (servo_10.read() < 90)
        servo_9.write(servo_9.read() - map(dx, 0, -50, 0, 6));
      else
        servo_9.write(servo_9.read() + map(dx, 0, -50, 0, 6));
    } else if (dx > ss.getWidth()/2) {
      if (servo_10.read() < 90)
        servo_9.write(servo_9.read() + map(dx, 0, 50, 0, 6));
      else
        servo_9.write(servo_9.read() - map(dx, 0, 50, 0, 6));
    }
    if (dy < ss.getHeight()/-2) {
      servo_10.write(servo_10.read() - map(dy, -10, -50, 0, 4)); // turn up
    } else if (dy > ss.getHeight()/2) {
      servo_10.write(servo_10.read() + map(dy, 10, 50, 0, 4)); // turn down
    }
  }
  // delay(20);
}
