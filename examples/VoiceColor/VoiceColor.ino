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

bool bVoice = true;

void setup()
{
  //Serial.begin(9600);
  Serial.println("setup");
	ss.begin();

  ss.enableFunc(Pixetto::FUNC_VOICE_COMMAND);
  bVoice = true;
	pinMode(11, OUTPUT);
  pinMode(13, OUTPUT);

  digitalWrite(13, HIGH);
  digitalWrite(11, HIGH);

}


void loop()
{
	if (ss.isDetected()) {
		// Available voice commands are defined in Pixetto.h.
		if (bVoice == true && ss.getFuncID() == Pixetto::FUNC_VOICE_COMMAND) {
      Serial.print("Voice Command  ");
      Serial.println(ss.getTypeID());
			if (ss.getTypeID() == Pixetto::VOICE_WhatColor) {
        ss.enableFunc(Pixetto::FUNC_COLOR_DETECTION);
        bVoice = false;
        digitalWrite(13, LOW);
        digitalWrite(11, LOW);
      }
      if (ss.getTypeID() == Pixetto::VOICE_WhatShape) {
        ss.enableFunc(Pixetto::FUNC_SHAPE_DETECTION);
        bVoice = false;
        digitalWrite(13, LOW);
        digitalWrite(11, LOW);
      }
    }

    if (bVoice == false && ss.getFuncID() == Pixetto::FUNC_COLOR_DETECTION) {
      Serial.print("Color Detection  ");
      Serial.println(ss.getTypeID());
      if (ss.getTypeID() == Pixetto::COLOR_RED) {
				digitalWrite(13, HIGH);
        digitalWrite(11, LOW);
        delay(2000);
      }
      else if (ss.getTypeID() == Pixetto::COLOR_BLUE) {
        digitalWrite(11, HIGH);
        digitalWrite(13, LOW);
        delay(2000);
      }
      ss.enableFunc(Pixetto::FUNC_VOICE_COMMAND);
      bVoice = true;
      digitalWrite(13, HIGH);
      digitalWrite(11, HIGH);
    }
	}
	//delay(20);
}
