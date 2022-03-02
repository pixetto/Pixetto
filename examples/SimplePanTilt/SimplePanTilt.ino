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

const int arc_x_min    = 6;    // left
const int arc_x_center = 80;   // center
const int arc_x_max    = 154;  // right

const int arc_y_min    = 0;    // down
const int arc_y_center = 30;   // center
const int arc_y_max    = 60;   // up

const int rxPin = 0;
const int txPin = 1;

Pixetto ss(rxPin, txPin);

Servo servo_9;
Servo servo_10;

void setup()
{
  servo_9.attach(9);
  servo_10.attach(10);

  servo_9.write(arc_x_center);
  servo_10.write(arc_y_center);

  ss.begin();
  ss.enableFunc(Pixetto::FUNC_COLOR_DETECTION);
}

void loop()
{
  if (ss.isDetected()) {
    int x = ss.getPosX() + ss.getWidth() / 2;
    int dx = x - 50;
    int y = ss.getPosY() + ss.getHeight() / 2;
    int dy = y - 50;

    int arc_x = servo_9.read();
    if (dx < ss.getWidth() / -2) {
      arc_x = servo_9.read() + map(dx, 0, -50, 1, 12); // turn left
    } else if (dx > ss.getWidth() / 2) {
      arc_x = servo_9.read() - map(dx, 0, 50, 1, 12); // turn right
    }
    arc_x = constrain(arc_x, arc_x_min, arc_x_max);
    servo_9.write(arc_x);

    int arc_y = servo_10.read();
    if (dy < ss.getHeight() / -2) {
      arc_y = servo_10.read() + map(dy, 0, -50, 1, 6); // turn up
    } else if (dy > ss.getHeight() / 2) {
      arc_y = servo_10.read() - map(dy, 0, 50, 1, 6); // turn down;
    }
    arc_y = constrain(arc_y, arc_y_min, arc_y_max);
    servo_10.write(arc_y);
  }
}
