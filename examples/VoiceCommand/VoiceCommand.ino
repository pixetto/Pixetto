/*
 * Copyright 2020 VIA Technologies, Inc. All Rights Reserved.
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
#define rxPin 0
#define txPin 1

// SoftwareSerial pins
//#define rxPin A5
//#define txPin A4

Pixetto ss(rxPin, txPin);

int led = 13;

void setup()
{
  ss.begin();
  ss.enableFunc(Pixetto::FUNC_VOICE_COMMAND);

  pinMode(led, OUTPUT);
}

void loop()
{
  if (ss.isDetected()) {
    // Available voice commands are defined in Pixetto.h.
    if (ss.getFuncID() == Pixetto::FUNC_VOICE_COMMAND) {
      if (ss.getTypeID() == Pixetto::VOICE_TurnOnLight) {
	digitalWrite(led, HIGH);
      } else if (ss.getTypeID() == Pixetto::VOICE_TurnOffLight) {
	digitalWrite(led, LOW);
      }
    }
  }
  //delay(20);
}
