/*
 * Copyright 2017 VIA Technologies, Inc. All Rights Reserved.
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
#include <Wire.h>
#include <Dino.h>

int result;
int pins[] = {3, 5, 6, 9, 10, 11};

Dino ai;

void setup()
{
  Wire.begin();

  result = 0;
  ai.init();
  ai.note_detection(true);
}


void loop()
{
  result = ai.poll();
  if (result == 133) {
    for (int i = 0; i <= 5; i++) {
      pinMode((pins[i]), OUTPUT);
      analogWrite((pins[i]), ai.note(i));
    }
  }
}
