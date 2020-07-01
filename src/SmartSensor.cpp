/*
 * Copyright 2020 VIA Technologies, Inc. All Rights Reserved.
 *
 * This PROPRIETARY SOFTWARE is the property of VIA Technologies, Inc.
 * and may contain trade secrets and/or other confidential information of
 * VIA Technologies, Inc. This file shall not be disclosed to any third
 * party, in whole or in part, without prior written consent of VIA.
 *
 * THIS PROPRIETARY SOFTWARE AND ANY RELATED DOCUMENTATION ARE PROVIDED AS IS,
 * WITH ALL FAULTS, AND WITHOUT WARRANTY OF ANY KIND EITHER EXPRESS OR IMPLIED,
 * AND VIA TECHNOLOGIES, INC. DISCLAIMS ALL EXPRESS OR IMPLIED
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET
 * ENJOYMENT OR NON-INFRINGEMENT.
*/

#include <Arduino.h>
#include <SmartSensor.h>

#define SS_FUNC(f, func, ...) \
	if (f == true) \
		return ss_hw->func(__VA_ARGS__); \
	else \
		return ss_sw->func(__VA_ARGS__);


SmartSensor::SmartSensor(int rx, int tx)
	: swSer(0), ss_hw(0), ss_sw(0), m_flag(false)
{
	// https://www.arduino.cc/reference/en/language/functions/communication/serial/

#if defined(HAVE_HWSERIAL0) && defined(HAVE_HWSERIAL1) && defined(HAVE_HWSERIAL2) && defined(HAVE_HWSERIAL3)
	// Mega, Due
	if (rx == 0 && tx == 1)
	{
		m_flag = true;
		ss_hw = new InnerSensor<HardwareSerial>(&Serial);
		return;
	}
	if (rx == 19 && tx == 18)
	{
		m_flag = true;
		ss_hw = new InnerSensor<HardwareSerial>(&Serial1);
		return;
	}
	if (rx == 17 && tx == 16)
	{
		m_flag = true;
		ss_hw = new InnerSensor<HardwareSerial>(&Serial2);
		return;
	}
	if (rx == 15 && tx == 14)
	{
		m_flag = true;
		ss_hw = new InnerSensor<HardwareSerial>(&Serial3);
		return;
	}
#elif defined(HAVE_HWSERIAL1)
	// Leonardo, Microm Yun, MKR, Zero, 101
	if (rx == 0 && tx == 1)
	{
		m_flag = true;
		ss_hw = new InnerSensor<HardwareSerial>(&Serial1);
		return;
	}
	if (rx == 13 && tx == 14)
	{
		m_flag = true;
		ss_hw = new InnerSensor<HardwareSerial>(&Serial1);
		return;
	}
#elif defined(HAVE_HWSERIAL0)
	// Uno, Nano, Mini
	if (rx == 0 && tx == 1)
	{
		m_flag = true;
		ss_hw = new InnerSensor<HardwareSerial>(&Serial);
		return;
	}
#endif

	m_flag = false;
	swSer = new SoftwareSerial(rx,tx);
	ss_sw = new InnerSensor<SoftwareSerial>(swSer);
}

SmartSensor::~SmartSensor()
{
	if (ss_hw)
	{
		delete ss_hw;
		ss_hw = 0;
	}
	if (ss_sw)
	{
		delete ss_sw;
		ss_sw = 0;
	}
	if (swSer)
	{
		delete swSer;
		swSer = 0;
	}
}

void SmartSensor::enableUVC(bool uvc)
{
	SS_FUNC(m_flag, enableUVC, uvc)
}

void SmartSensor::begin()
{
	SS_FUNC(m_flag, begin)
}

void SmartSensor::end()
{
	SS_FUNC(m_flag, end)
}

bool SmartSensor::isDetected()
{
	SS_FUNC(m_flag, isDetected)
}

int SmartSensor::getFuncID()
{
	SS_FUNC(m_flag, getFuncID)
}

int SmartSensor::getTypeID()
{
	SS_FUNC(m_flag, getTypeID)
}

int SmartSensor::getPosX()
{
	SS_FUNC(m_flag, getPosX)
}

int SmartSensor::getPosY()
{
	SS_FUNC(m_flag, getPosY)
}

int SmartSensor::getH()
{
	SS_FUNC(m_flag, getH)
}

int SmartSensor::getW()
{
	SS_FUNC(m_flag, getW)
}

int SmartSensor::getHeight()
{
	SS_FUNC(m_flag, getH)
}

int SmartSensor::getWidth()
{
	SS_FUNC(m_flag, getW)
}

int SmartSensor::numObjects()
{
	SS_FUNC(m_flag, numObjects)
}

void SmartSensor::getLanePoints(int* lx1, int* ly1, int* lx2, int* ly2, int* rx1, int* ry1, int* rx2, int* ry2)
{
	SS_FUNC(m_flag, getLanePoints, lx1, ly1, lx2, ly2, rx1, ry1, rx2, ry2)
}

void SmartSensor::getEquationExpr(char *buf, int len)
{
	SS_FUNC(m_flag, getEquationExpr, buf, len)
}

float SmartSensor::getEquationAnswer()
{
	SS_FUNC(m_flag, getEquationAnswer)
}
