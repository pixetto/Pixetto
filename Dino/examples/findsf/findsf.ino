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
  ai.add_sentence("ni hao", 1);
  ai.add_sentence("xie xie", 2);
  ai.add_sentence("zai jian", 3);
  ai.add_sentence("kai deng", 4);
  ai.add_sentence("guan deng", 5);
  ai.add_sentence("chuang zao li", 6);
  ai.add_sentence("zhe shi she me", 7);
  ai.enroll("dan.wav", 8);
  ai.enroll("tina.wav", 9);
  ai.enroll("sylvia.wav", 10);
  ai.enroll("rebecca.wav", 11);
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
    ai.speak("在");
    ai.echo();
    break;
   case 7:
    ai.shell("look");
    break;
   case 30546:
    ai.speak("发现电风扇。");
    break;
   case 8:
    ai.speak("嗨！丹。");
    break;
   case 9:
    ai.speak("嗨！蒂娜。");
    break;
   case 10:
    ai.speak("嗨！丝丽维亚。");
    break;
   case 11:
    ai.speak("嗨！瑞贝卡。");
    break;
  }
  delay(100);

}
