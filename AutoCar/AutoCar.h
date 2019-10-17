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
#ifndef AutoCar_h
#define AutoCar_h
#define DINO_VERSION 1.7.1

#include <inttypes.h>
#include <Servo.h>
#include "Arduino.h"

class AutoCar
{
public:
	AutoCar();
	~AutoCar();
	static uint8_t i2c_addr() { return 0x31; };
	
	struct ROADSIGN {
		int type;
		int x;
		int y;
		int width;
		int height;
		int src_width;
		int src_height;
		int ratio;
		int l_distance;
		int r_distance;
	} detectedSign;
	bool probe();
	bool signDetected();
	//int getDetectedSign();
	int getSignType();
	int getXCoordinate();
	int getYCoordinate();
	int getSignWidth();
	int getSignHeight();
	int getCameraWidth();
	int getCameraHeight();
	int getSignRatio();
	int getLDistance();
	int getRDistance();
	void drive(int speed);
	void brake();
	void turnLeft(int angle);
	void turnRight(int angle);
	void goStraight();
	void forward();
	void backward();
	void init();
	void initMLPModule();
	void initEdgeDetectModule();
	void initHoughModule();
	void configMotor(int num, int pwm, int dir);
	void configServo(int pin);
	void configDriverless(int enable);


private:
	int read(void* buf, size_t count);
	int write(const void* buf, size_t count);
	int recv(void* buf, size_t count);
	int send(const void* buf, size_t count);
	uint8_t readb();
	int sendmsg(const char* msg);
	void setMotor(int m1Speed);
	
	int devBooted;
	int driverlessEnabled;
	int probe_results;
	int lcount;
	int rcount;
	int pin_pwmA;
	int pin_dirA;
	int pin_pwmB;
	int pin_dirB;
	int pin_servo;
	int m_ld;
	int m_rd;
};

#endif
