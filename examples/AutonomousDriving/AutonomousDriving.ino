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
	//Serial.begin(9600);
	ss.begin();
	
	ss.enableFunc(Pixetto::FUNC_AUTONOMOUS_DRIVING);
	//ss.enableUVC(true);
	
	ss.setDetectMode(false);
}


void loop()
{
	int x1,y1,x2,y2,x3,y3,x4,y4,cx,cy;
	int typeID;
	
	if (ss.isDetected()) {
		if (ss.getFuncID() == Pixetto::FUNC_AUTONOMOUS_DRIVING) {
	
			// Lane points are valid only when (ss.getPosX() >= 0)
			cx = ss.getPosX();
			if (cx >= 0) {
				cy = ss.getPosY();
				String a = "(" + String(cx) + "," + String(cy) + ")";
				Serial.print(a);
				ss.getLanePoints(&x1,&y1,&x2,&y2,&x3,&y3,&x4,&y4);
				String b = "(" + String(x1) + "," + String(y1) + ")("+ String(x2) + "," + String(y2) + ")("+ String(x3) + "," + String(y3) + ")("+ String(x4) + "," + String(y4) + ")";
				Serial.println(b);
			}
	
			// Type ID (traffic sign ID) is valid only when (ss.getTypeID() >= 0)
			typeID = ss.getTypeID();
			if (typeID >= 0) {
				Serial.print("Sign ID=");
				Serial.println(typeID);
			}
		}
	}
	//delay(20);
}
