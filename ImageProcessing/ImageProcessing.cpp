/*
 * Copyright 2019 VIA Technologies, Inc. All Rights Reserved.
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
#include "ImageProcessing.h"
#include <avr/wdt.h>

ImageProcessing::ImageProcessing()
{
	Serial.begin(9600);
}

ImageProcessing::~ImageProcessing()
{
}

void ImageProcessing::init()
{
	sendmsg("imageP");
	delay(300);
}

uint8_t ImageProcessing::readb()
{
	uint8_t c = 0;
	int i;
	int ttl = 256;

	do {
		i = read(&c, 1);
	} while (i == 0 && --ttl);

	return c;
}

int ImageProcessing::read(void* buf, size_t count)
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

int ImageProcessing::write(const void* buf, size_t count)
{
	Wire.beginTransmission(i2c_addr());

	const char* p = (const char*)buf;
	int i = count;
	while (i--)
		Wire.write(*p++);

	Wire.endTransmission();

	return count;
}

int ImageProcessing::recv(void* buf, size_t count)
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

int ImageProcessing::send(const void* buf, size_t count)
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

int ImageProcessing::sendmsg(const char* msg)
{
	int len = strlen(msg);

	send((const void*)msg, len);

	return len;
}

bool ImageProcessing::poll()
{
	char buf[BUFLEN];
	char *pch;
	int val;

	int n = recv(buf, BUFLEN);
	buf[n] = '\0';

	if (n == 0)
		return -1;

	bool ret = false;

	num_result = 0;
	memset(result, 0, 32);

	if(strncmp(buf, "#reset", n) == 0) {
		wdt_enable(WDTO_15MS);
		while (true) {};
	}

	if(strstr(buf, "ret") != NULL) {
		int i = 0;
		int j = 0;
		pch = strtok(buf, ",");
		while(pch != NULL) {
			if(i > 0) {
				val = atoi(pch);
				if(i==1) {
					num_result = val;
				}
				else {
					result[j] = val;
					j++;
				}
			}
			i++;
			pch = strtok(NULL, ",");
			
		}
		ret = true;
	}

	return ret;
}

int ImageProcessing::getIndexOfResult(int num)
{
	return result[num];
}

int ImageProcessing::getNumOfResult()
{
	return num_result;
}

bool ImageProcessing::detect(int mode)
{
	int i = 0;
	bool timeout = false;

	switch(mode) {
		case MODE_COLOR:
			sendmsg("ColorDetector");
			break;
		case MODE_FACE:
			sendmsg("FaceDetector");
			break;
		case MODE_LETTERS:
			sendmsg("LettersDetector");
			break;
		case MODE_DIGITS:
			sendmsg("DigitsDetector");
			break;
	}

	bool ret = false;
	do {
		ret = poll();
		delay(30);
		i++;
		if(i > 20)
			timeout = true;
	} while(!ret && !timeout);

	return ret;
}
