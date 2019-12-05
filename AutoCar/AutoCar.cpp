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
#include "AutoCar.h"
#include <avr/wdt.h>

#define SYN 0x16
#define STX 0x02
#define ETX 0x03

#define S_TYPE		1
#define S_X			2
#define S_Y			3
#define S_WIDTH		4
#define S_HEIGHT	5
#define S_SWIDTH	6
#define S_SHEIGHT	7
#define S_RATIO		8
#define S_LDIST		9
#define S_RDIST		10

#define BUFLEN 128

#define SERVOMIN 88//the angle of servo
#define SERVOMID 90
#define SERVOMAX 108

Servo myservo;
int straight_speed = 45;
int curve_speed = 50;
int lastSign = 0;

AutoCar::AutoCar()
{
	
}

AutoCar::~AutoCar()
{

}

uint8_t AutoCar::readb()
{
	uint8_t c = 0;
	int i;
	int ttl = 256;

	do {
		i = read(&c, 1);
	} while (i == 0 && --ttl);

	return c;
}

int AutoCar::read(void* buf, size_t count)
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

int AutoCar::write(const void* buf, size_t count)
{
	Wire.beginTransmission(i2c_addr());

	const char* p = (const char*)buf;
	int i = count;
	while (i--)
		Wire.write(*p++);

	Wire.endTransmission();

	return count;
}

int AutoCar::recv(void* buf, size_t count)
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

int AutoCar::send(const void* buf, size_t count)
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

int AutoCar::sendmsg(const char* msg)
{
	int len = strlen(msg);

	send((const void*)msg, len);

	return len;
}

bool AutoCar::probe()
{
	char buf[BUFLEN];
	char tmp[BUFLEN];
	char *pch;
	int i = 0, n = 0;
	int count = 0;
	int val;

	probe_results = false;
	detectedSign = {0};
	if(!driverlessEnabled)
		return probe_results;

	sendmsg("signdetect");
	while(1)
	{
		n = recv(buf, BUFLEN);
		if(n == 0) {
			delay(10);
			count++;
			if(count >= 20)
				return probe_results;
			else
				continue;
		}
		if (strncmp(buf, "#reset", n) == 0) {
			wdt_enable(WDTO_15MS);
			while (true) {};
		}
		if(strstr(buf, "ret") != NULL)
			break;
	}

    devBooted = 1;
	buf[n] = '\0';
	pch = strtok(buf, ",");

	while(pch != NULL)
	{
		val = atoi(pch);
		switch (i)
		{
			case S_TYPE:
				if(lastSign == val)
					detectedSign.type = val;
				else
					lastSign = val;
				break;
			case S_X:
				detectedSign.x = val;
				break;
			case S_Y:
				detectedSign.y = val;
				break;
			case S_WIDTH:
				detectedSign.width = val;
				break;
			case S_HEIGHT:
				detectedSign.height = val;
				break;
			case S_SWIDTH:
				detectedSign.src_width = val;
				break;
			case S_SHEIGHT:
				detectedSign.src_height = val;
				break;
			case S_RATIO:
				detectedSign.ratio = val;
				break;
			case S_LDIST:
				detectedSign.l_distance = val;
				m_ld = val;
				break;
			case S_RDIST:
				detectedSign.r_distance = val;
				m_rd = val;
				break;
		}
		i++;
		pch = strtok(NULL, ",");
	}
	float ld = detectedSign.l_distance;
	float rd = detectedSign.r_distance;
	if(rd>0 && ld>0) {
		if(rd!=999 && ld!=999) {
			detectedSign.l_distance = ld/(ld+rd)*100;
			detectedSign.r_distance = rd/(ld+rd)*100;
		}
		else {
			if(ld==999 && rd==999) {
				detectedSign.r_distance = -1;
				detectedSign.l_distance = -1;
			}
			else {
				if(rd != 999) {
					detectedSign.r_distance = rd / 160 * 100;
					detectedSign.l_distance = 100 - detectedSign.r_distance;
				}
				else {
					detectedSign.l_distance = ld / 160 * 100;
					detectedSign.r_distance = 100 - detectedSign.l_distance;
				}
			}
		}
	}
	else {
		if(rd == 0)
			detectedSign.l_distance = 100;
		else
			detectedSign.r_distance = 100;
	}

	probe_results = true;
	return probe_results;
}

bool AutoCar::signDetected()
{
	return probe_results;
}

int AutoCar::getSignType()
{
	return this->detectedSign.type;
}

int AutoCar::getXCoordinate()
{
	return this->detectedSign.x;
}

int AutoCar::getYCoordinate()
{
	return this->detectedSign.y;
}

int AutoCar::getSignWidth()
{
	return this->detectedSign.width;
}

int AutoCar::getSignHeight()
{
	return this->detectedSign.height;
}

int AutoCar::getCameraWidth()
{
	return this->detectedSign.src_width;
}

int AutoCar::getCameraHeight()
{
	return this->detectedSign.src_height;
}

int AutoCar::getSignRatio()
{
	return this->detectedSign.ratio;
}

int AutoCar::getLDistance()
{
	return this->detectedSign.l_distance;
}

int AutoCar::getRDistance()
{
	return this->detectedSign.r_distance;
}

void AutoCar::setMotor(int m1Speed)
{
	if (m1Speed > 0) {
		digitalWrite(pin_dirA, LOW);
		analogWrite(pin_pwmA, 255 + m1Speed);
		digitalWrite(pin_dirB, LOW);
		analogWrite(pin_pwmB, 255 + m1Speed);
	}
	else if(m1Speed < 0) {
		digitalWrite(pin_dirA, HIGH);
		analogWrite(pin_pwmA, 255 - m1Speed);
		digitalWrite(pin_dirB, HIGH);
		analogWrite(pin_pwmB, 255 - m1Speed);
	}
	else {
		analogWrite(pin_pwmA, 255 + 30);
		analogWrite(pin_pwmB, 255 + 30);
		analogWrite(pin_pwmA, 255 + 20);
		analogWrite(pin_pwmB, 255 + 20);
		analogWrite(pin_pwmA, 255 + 10);
		analogWrite(pin_pwmB, 255 + 10);
		analogWrite(pin_pwmA, 0);
		analogWrite(pin_pwmB, 0);
		delay(300);

		digitalWrite(pin_pwmA, LOW);
		digitalWrite(pin_dirA, HIGH);
		digitalWrite(pin_pwmB, LOW);
		digitalWrite(pin_dirB, HIGH);
	}
}

void AutoCar::drive(int speed)
{
	straight_speed = speed;
	curve_speed = straight_speed + 5;
	if(driverlessEnabled) {
		if(devBooted)
			setMotor(speed);
	}
	else
		setMotor(speed);
}

void AutoCar::brake()
{
	setMotor(0);
}

void AutoCar::turnLeft(int angle)
{
	myservo.write(SERVOMID - angle);
}

void AutoCar::turnRight(int angle)
{
	myservo.write(SERVOMID + angle);
}

void AutoCar::goStraight()
{
	myservo.write(SERVOMID);
}

void AutoCar::init()
{
	driverlessEnabled = 0;
	devBooted = 0;
	probe_results = -1;
	sendmsg("autocar");
	delay(300);
}

void AutoCar::configMotor(int num, int pwm, int dir)
{
	if(num == 0) {
		pin_pwmA = pwm;
		pin_dirA = dir;
		pinMode(pin_pwmA, OUTPUT);
		pinMode(pin_dirA, OUTPUT);
	}
	else if(num == 1) {
		pin_pwmB = pwm;
		pin_dirB = dir;
		pinMode(pin_pwmB, OUTPUT);
		pinMode(pin_dirB, OUTPUT);
	}
}

void AutoCar::configServo(int pin)
{
	pin_servo = pin;
	myservo.attach(pin_servo);
	myservo.write(98);
}

void AutoCar::configDriverless(int enable)
{
	driverlessEnabled = enable;
}

void AutoCar::forward()
{
	if(driverlessEnabled) {
		int ld = m_ld;
		int rd = m_rd;
		int limit = 80;//40

		if(ld>rd) {
			if(rd<limit) {
				setMotor(curve_speed);
				if(rd == 0)
					turnLeft(50);
				else
					turnLeft(25);
			}
			else {
				setMotor(straight_speed);
				goStraight();
			}
		}
		else {
			if(ld<limit) {
				setMotor(curve_speed);
				if(ld == 0)
					turnRight(30);
				else
					turnRight(15);
			}
			else {
				setMotor(straight_speed);
				goStraight();
			}
		}
	}
}

void AutoCar::backward()
{
	setMotor(-40);
}

void AutoCar::initMLPModule()
{
	sendmsg("initMLP");
}

void AutoCar::initEdgeDetectModule()
{
	sendmsg("initEdges");
}

void AutoCar::initHoughModule()
{
	sendmsg("initHough");
}
