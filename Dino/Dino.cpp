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
#include "Arduino.h"
#include "Wire.h"
#include "Dino.h"
#include <avr/wdt.h>

#define SYN 0x16
#define STX 0x02
#define ETX 0x03

#define BUFLEN 128

#define OP_NONE   0
#define OP_LISTEN 1
#define OP_RECORD 2
#define OP_NOTE_DETECTION 3

Dino::Dino()
: index_(1), op_(OP_NONE)
{
	wdt_disable();

}

Dino::~Dino()
{

}

void Dino::init()
{
	sendmsg("voice");
	String msg = ":init";
	sendmsg(msg.c_str());

	sendmsg(":!listen");
	sendmsg(":!nd");
	captureVolume(255);
	playbackVolume(255);
	sensitivity(255);
	language("zh_CN");

	human_voice_frequency_ = -1;
}

void Dino::name(String text)
{
	name_ = text;
	text.toLowerCase();
	String msg = ":name," + text;
	sendmsg(msg.c_str());
}

void Dino::learn(String sentence)
{
	sentence.toLowerCase();
	String msg = ":learn," + sentence + "," + index_;
	sendmsg(msg.c_str());
	index_++;
}

void Dino::learn(String sentence, int id)
{
	sentence.toLowerCase();
	String msg = ":learn," + sentence + "," + id;
	sendmsg(msg.c_str());
}

void Dino::add_sentence(String sentence, int id)
{
	learn(sentence, id);
}

void Dino::train()
{
	String msg = ":train";
	sendmsg(msg.c_str());
}

void Dino::enroll(String wavfile, int id)
{
	String msg = ":enroll," + wavfile + "," + id;
	sendmsg(msg.c_str());
}

void Dino::identify()
{
	String msg = ":identify";
	sendmsg(msg.c_str());
}

signed int Dino::poll()
{
	char buf[BUFLEN];
	int n = recv(buf, BUFLEN);
	buf[n] = '\0';

	if (n == 0)
		return -1;

	int ret = -1;

	switch (buf[0]) {
	case '=':
		ret = atoi(&buf[1]);
		break;
	case '#':
		if (strncmp(buf, "#reset", n) == 0) {
			wdt_enable(WDTO_15MS);
			while (true) {};
		}
		break;
	case 'H':
		human_voice_frequency_ = atoi(&buf[1]);
		break;
	case 'N':
		for (int i = 0; i < 7; i++) {
			int lv = buf[i+1] - 0x30;
			note_[i] = lv ? (lv * 32 - 1) : 0;
		}
		ret = 133;
		break;
	case '%':
		pinMode(13, OUTPUT);
		if(strncmp(buf, "%blinkLED", n) == 0) {
			digitalWrite(13, HIGH);
			delay(1000);
			digitalWrite(13, LOW);
			delay(1000);
		}
		else if(strncmp(buf, "%LOW", n) == 0) {
			digitalWrite(13, LOW);
		}
		else if(strncmp(buf, "%HIGH", n) == 0) {
			digitalWrite(13, HIGH);
		}
		break;
	default:
		sendmsg(buf);
		break;
	}
	// sendmsg(buf);

	if (ret == 128) // IDLE
		is_busy_ = false;

	if (ret == 129) // BUSY
		is_busy_ = true;

	return ret;
}

uint8_t Dino::readb()
{
	uint8_t c = 0;
	int i;
	int ttl = 256;

	do {
		i = read(&c, 1);
	} while (i == 0 && --ttl);

	return c;
}

int Dino::read(void* buf, size_t count)
{
	int i = 0;
	Wire.requestFrom(i2c_addr(), count);
	char* p = (char*)buf;
	while (Wire.available()) {
		*p++ = Wire.read();
		i++;
	}
	return i;
}

int Dino::write(const void* buf, size_t count)
{
	Wire.beginTransmission(i2c_addr());

	const char* p = (const char*)buf;
	int i = count;
	while (i--)
		Wire.write(*p++);

	Wire.endTransmission();

	return count;
}

int Dino::recv(void* buf, size_t count)
{
	uint8_t c = 0;

	if (buf == NULL || count == 0)
		return 0;

	/* SYN */

	c = readb();

	if (c != SYN)
		return 0;

	/* SYN */

	c = readb();

	if (c != SYN)
		return 0;

	/* STX */

	c = readb();

	if (c != STX)
		return 0;

	/* DATA */

	uint8_t *p = (uint8_t*)buf;
	size_t len = 0;

	while (len < count) {
		c = readb();
		if (c == 0)
			return 0;
		if (c == ETX)
			return len;
		*p++ = c;
		len++;
	};

	return len;
}

int Dino::send(const void* buf, size_t count)
{
	Wire.beginTransmission(0x31);
	Wire.write(SYN);
	Wire.write(SYN);
	Wire.write(STX);

	const char* p = (const char*)buf;
	int i = count;
	int j = 0;
	while (i--) {
		Wire.write(*p++);
		j++;
		if (j >= 16) {
			Wire.endTransmission();
			Wire.beginTransmission(0x31);
			j = 0;
		}
	}

	Wire.write(ETX);
	Wire.endTransmission();

	return count;
}

int Dino::sendmsg(const char* msg)
{
	int len = strlen(msg);

	send((const void*)msg, len);

	return len;
}

bool Dino::isbusy()
{
	return is_busy_;
}

void Dino::println(String text)
{
	String msg = ":println," + text;
	sendmsg(msg.c_str());
}

void Dino::shell(String text)
{
	String msg = "#" + text;
	sendmsg(msg.c_str());
}

void Dino::playbackVolume(size_t value)
{
	char s[BUFLEN];

	value = constrain(value, 0, 255) & ~0x7;

	if (playback_volume_ == value)
		return;

	size_t rate = map(value, 0, 255, 0, 100);

	sprintf(s, ":mixer,p,%d%%", rate);
	sendmsg(s);

	playback_volume_ = value;
}

void Dino::captureVolume(size_t value)
{
	char s[BUFLEN];

	value = constrain(value, 0, 255) & ~0x7;

	if (capture_volume_ == value)
		return;

	size_t rate = map(value, 0, 255, 0, 100);

	sprintf(s, ":mixer,c,%d%%", rate);
	sendmsg(s);

	capture_volume_ = value;
}

void Dino::sensitivity(size_t value)
{
	char s[BUFLEN];

	value = constrain(value, 0, 255) & ~0x7;

	if (threshold_ == value)
		return;

	sprintf(s, ":sensitivity,%d", value);
	sendmsg(s);

	threshold_ = value;
}

void Dino::language(String locale)
{
	// zh_CN, en_US
	language_ = locale;
}

void Dino::sync()
{
	delay(1000);
	int timeout = 300;
	do {
		poll();
		delay(200);
	} while (is_busy_ && --timeout);
}

void Dino::speak(String text)
{
	char str[BUFLEN];
	sprintf(str, ":speak,%s,%s", text.c_str(), language_.c_str());
	sendmsg(str);
	sync();
}

void Dino::play(String filename)
{
	String msg = ":play," + filename;
	sendmsg(msg.c_str());
	sync();
}

void Dino::listen()
{
	if (op_ == OP_NONE) {
		op_ = OP_LISTEN;
		sendmsg(":listen");
	}
}

void Dino::listenstop()
{
	if (op_ == OP_LISTEN) {
		op_ = OP_NONE;
		sendmsg(":!listen");
	}
}

void Dino::note_detection(bool on)
{
	if (on) {
		if (op_ == OP_NONE) {
			op_ = OP_NOTE_DETECTION;
			sendmsg(":nd");
		}
	} else {
		if (op_ == OP_NOTE_DETECTION) {
			op_ = OP_NONE;
			sendmsg(":!nd");
		}
	}
}

int Dino::note(int index)
{
	return note_[index % 7];
}

void Dino::record(String filename)
{
	if (op_ == OP_NONE) {
		op_ = OP_RECORD;
		String msg = ":record," + filename;
		sendmsg(msg.c_str());
	}
}

void Dino::recordstop()
{
	if (op_ == OP_RECORD) {
		op_ = OP_NONE;
		sendmsg(":!record");
	}
}

void Dino::echo()
{
	sendmsg(":echo");
	sync();
}

void Dino::generate_acoustic_models()
{
	sendmsg(":gam");
}

void Dino::generate_pronunciation_dictionary()
{
	sendmsg(":gpd");
}

void Dino::generate_language_models()
{
	sendmsg(":glm");
}

void Dino::generate_decoder()
{
	train();
}

void Dino::load_playlist()
{
	String s = "echo reset > /tmp/mp3cli";
	shell(s);
	delay(1000);
}

void Dino::play_track()
{
	String s = "echo continue > /tmp/mp3cli";
	shell(s);
	delay(1000);
}

void Dino::loop_track()
{
	String s = "echo loop > /tmp/mp3cli";
	shell(s);
	delay(1000);
}

void Dino::play_track_at(int location)
{
	char s[BUFLEN];
	sprintf(s, "echo location %d > /tmp/mp3cli", location);
	shell(s);
	delay(1000);
}

void Dino::play_next_track()
{
	String s = "echo forward > /tmp/mp3cli";
	shell(s);
	delay(1000);
}

void Dino::play_previous_track()
{
	String s = "echo backward > /tmp/mp3cli";
	shell(s);
	delay(1000);
}

void Dino::pause_track()
{
	String s = "echo pause > /tmp/mp3cli";
	shell(s);
	delay(1000);
}

void Dino::stop_track()
{
	String s = "echo stop > /tmp/mp3cli";
	shell(s);
	delay(1000);
}

int Dino::human_voice_frequency()
{
	if (human_voice_frequency_ < 0) {
		sendmsg(":hvf");
		human_voice_frequency_ = 0;
	}

	int ret = human_voice_frequency_;

	human_voice_frequency_ = 0;

	return ret;
}

bool Dino::is_speaking()
{
	return false;
};

void Dino::extract_features()
{
	sendmsg(":ef");
};

void Dino::decode(int am, int pd, int lm)
{
	sendmsg(":decode");
};
