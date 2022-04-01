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

// HardwareSerial pins
// #define rxPin 0
// #define txPin 1

// SoftwareSerial pins
#define rxPin A5
#define txPin A4

Pixetto ss(rxPin, txPin);

float tag_size = 8; //  (cm) tag length

void setup()
{
  Serial.begin(38400);

  ss.begin();
  ss.enableFunc(Pixetto::FUNC_APRILTAG);
}

void loop()
{
  if (ss.isDetected()) {
    if (ss.getFuncID() == Pixetto::FUNC_APRILTAG) {
      float px=0, py=0, pz=0;
      float rx=0, ry=0, rz=0;
      float cx=0, cy=0;
      ss.getApriltagInfo(&px, &py, &pz, &rx, &ry, &rz, &cx, &cy);
      int dist = sqrt(px*px + py*py + pz*pz) * tag_size;
      Serial.print("id=");
      Serial.print(ss.getTypeID());
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
}
