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
#ifndef Dino_h
#define Dino_h
#define DINO_VERSION 1.6.9

#include <inttypes.h>
#include "Arduino.h"

class Dino
{
public:
	Dino();
	~Dino();
	void init();
	void name(String text);
	void learn(String sentence);
	void learn(String sentence, int id);
	void train();
	signed int poll();
	static uint8_t i2c_addr() { return 0x31; };

	int sendmsg(const char* msg);
	bool isbusy();

	/* Play audio file */
	void play(String filename);

	/* Execute shell command */
	void shell(String text);

	/* Print message to SPI bus */
	void println(String text);

	/**
	 * Speaker volume
	 * @param value 0-255 (255)
	 */
	void playbackVolume(size_t value);

	/**
	 * Microphone volume
	 * @param value 0-255
	 */
	void captureVolume(size_t value);

	/**
	 * Sensitivity
	 * @param value 0-255
	 */
	void sensitivity(size_t value);

	/**
	 * Language
	 * @param lang "en_US", "zh_CN"
	 */
	void language(String locale);

	/**
	 * Text to speech
	 * @param text string in ascii, utf-8, or pinyin
	 * @param lang "en_US", "zh_CN"
	 */
	void speak(String text);

	/* Enable/disable speech to text */
	void listen();
	void listenstop();

	/* Enable/disable sound recording */
	void record(String filename);
	void recordstop();

	/* Call voice service */
	void echo();

	void add_sentence(String sentence, int id);
	void generate_acoustic_models();
	void generate_pronunciation_dictionary();
	void generate_language_models();
	void generate_decoder();

	void load_playlist();
	void play_track();
	void loop_track();
	void play_track_at(int location);
	void play_next_track();
	void play_previous_track();
	void pause_track();
	void stop_track();

	int  human_voice_frequency();
	void sync();

	bool is_speaking();
	void extract_features();
	void decode(int am, int pd, int lm);

	void enroll(String wavfile, int id);
	void identify();

	int acoustic_models;
	int pronunciation_dictionary;
	int language_models;

	void note_detection(bool on);
	int  note(int index);

private:
	String name_;
	int    index_;

	bool   is_busy_;

	size_t playback_volume_;
	size_t capture_volume_;
	size_t threshold_;

	int    op_;
	String language_;

	int read(void* buf, size_t count);
	int write(const void* buf, size_t count);
	int recv(void* buf, size_t count);
	int send(const void* buf, size_t count);
	uint8_t readb();

	int human_voice_frequency_;
	int note_[7]; /* Do Re Mi Fa So La Ti */
};

#endif
