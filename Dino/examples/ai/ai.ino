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

Dino ai;

void setup()
{
  Wire.begin();

  result = 0;
  ai.init();
  ai.language("zh_CN");
  ai.name("xiao yu");
  ai.add_sentence("ni hao", 1);
  ai.add_sentence("xie xie", 2);
  ai.add_sentence("zai jian", 3);
  ai.add_sentence("kai deng", 4);
  ai.add_sentence("guan deng", 5);
  ai.add_sentence("zai ma", 6);
  ai.add_sentence("zhe shi she me", 7);
  ai.add_sentence("zheng kai yan jing", 4);
  ai.add_sentence("zheng yan", 4);
  ai.add_sentence("bi shang yan jing", 5);
  ai.add_sentence("bi yan", 5);
  ai.enroll("male", 8);
  ai.enroll("female", 9);
  ai.generate_acoustic_models();
  ai.generate_pronunciation_dictionary();
  ai.generate_language_models();
  ai.generate_decoder();
  ai.sensitivity(255);
  ai.playbackVolume(255);
  ai.captureVolume(255);
  ai.listen();
  ai.speak("您好，很高兴为您服务。");
  pinMode(13, OUTPUT);
}


void loop()
{
  if (ai.is_speaking()) {
    ai.extract_features();
    ai.decode(
      ai.acoustic_models,
      ai.pronunciation_dictionary,
      ai.language_models);
  }
  result = ai.poll();
  switch (result) {
   case 1:
    ai.speak("您好，很高兴为您服务。");
    ai.identify();
    break;
   case 2:
    ai.speak("这是我应该做的。不用客气。");
    break;
   case 3:
    ai.speak("期待下次见面。再见。");
    break;
   case 4:
    digitalWrite(13, HIGH);
    break;
   case 5:
    digitalWrite(13, LOW);
    break;
   case 6:
    ai.listenstop();
    ai.speak("在");
    ai.echo();
    ai.listen();
    break;
   case 7:
    ai.shell("look");
    break;
   case 30546:
    ai.speak("发现电风扇。");
    break;
   case 8:
    ai.speak("男生");
    break;
   case 9:
    ai.speak("女生");
    break;
  }
  delay(100);
}
