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

// HardwareSerial pins
#define rxPin 0
#define txPin 1

// SoftwareSerial pins
//#define rxPin A5
//#define txPin A4

Pixetto ss(rxPin, txPin);

void setup()
{
	Serial.begin(9600);
	ss.begin();
	ss.setDetectMode(false);
	ss.enableFunc(Pixetto::FUNC_EQUATION_DETECTION);
}


void loop()
{
  int x,y,w,h;
  if (ss.isDetected()) {
    if (ss.getFuncID() == Pixetto::FUNC_EQUATION_DETECTION) {
      x = ss.getPosX();
      y = ss.getPosY();
      w = ss.getW();
      h = ss.getH();

      String b = "x=" + String(x) + " y=" + String(y) + " w="+ String(w) + " h=" + String(h);
      Serial.println(b);

      float ans = ss.getEquationAnswer();
      char szop[32];
      ss.getEquationExpr(szop, sizeof(szop));

      Serial.print("Answer=");
      Serial.print(String(ans));
      Serial.print("  Expr=");
      Serial.println(szop);
      Serial.println();
    }
  }
	delay(20);

}
