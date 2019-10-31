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
#include <ArduinoJson.h>
#include <SmartSensor.h>

#define SENSOR_CMD_STREAMON  "{\"header\":\"STREAMON\"};"
//#define SENSOR_CMD_QUERY	 "{\"header\":\"QUERY\"};"

#define MAX_OPENCAM_ERROR   7
#define MAX_JSON_ERROR   	7

SmartSensor::SmartSensor(SoftwareSerial *_swSer)
 : m_id(0), m_type(0),
   m_x(0), m_y(0), m_h(0), m_w(0),
   swSerial(_swSer),
   isCamOpened(false), hasDelayed(false), bSendStreamOn(false),
   nJsonErrCount(0), nOpenCamFailCount(0)
{

}

SmartSensor::~SmartSensor()
{

}

void SmartSensor::serialFlush()
{
	while (swSerial->available() > 0)
	    char t = swSerial->read();

}

void SmartSensor::begin()
{
    swSerial->begin(38400);
    hasDelayed = false;
    isCamOpened = false;
    bSendStreamOn = false;
    nOpenCamFailCount = 0;
    nJsonErrCount = 0;
}

void SmartSensor::end()
{
    swSerial->end();
}

bool SmartSensor::openCam()
{
	if (isCamOpened)
        return true;

	if (!hasDelayed)
	{
		delay(4000);
		hasDelayed = true;
	}
	else
	    delay(1000);
	    
	if (!bSendStreamOn)
	{
	    serialFlush();
	    // If it has not received response for the previous streamon cmd yet,
	    // do not send streamon command again.
		swSerial->print(SENSOR_CMD_STREAMON);
        Serial.println("send: STREAMON");
		bSendStreamOn = true;
 	}

    if (swSerial->available() > 0)
    {
        bSendStreamOn = false; // After receiving response, reset the flag

        DynamicJsonDocument doc(256);
		String s = swSerial->readStringUntil(';');
		//Serial.print("recv:");
		Serial.println(s);
		DeserializationError error = deserializeJson(doc, s);
		if (error)
		{
		    Serial.println("STREAMON CMD error!!");
		    return false;
		}

		String hdr = doc["header"];
		if (hdr == "CAM_SUCCESS")
		{
		    Serial.println("STREAMON OK!!");
		    isCamOpened = true;
		    return true;
 		}
		else
		    Serial.println("STREAMON Failed!!");
	}
	else
	{
	    nOpenCamFailCount++;
	 	if (nOpenCamFailCount > MAX_OPENCAM_ERROR)
	 	{
	 	    bSendStreamOn = false;
	 	    nOpenCamFailCount = 0;
		}
	}
	return false;
}

bool SmartSensor::isDetected()
{
	bool ret = openCam();
    if (!ret)
    {
   	    Serial.println("openCam() failed");
        return false;
	}
	//else
	    //Serial.println("openCam() OK");


    //serialFlush();
	//swSerial->print(SENSOR_CMD_QUERY);
	//Serial.println("send QUERY");

	DynamicJsonDocument doc(256);

    if (swSerial->available() > 0)
    {
		String s = swSerial->readStringUntil(';');
		Serial.println(s);

		DeserializationError error = deserializeJson(doc, s);
		if (error)
		{
		    nJsonErrCount++;
		    Serial.print("JSON Error:");
		    Serial.println(nJsonErrCount);
		    if (nJsonErrCount > MAX_JSON_ERROR)
		    {
		        this->end();
		        delay(50);
		        swSerial->begin(115200);
		    	swSerial->print("reset\n");
		    	swSerial->print("reset\n");
		    	swSerial->print("reset\n");
		    	swSerial->flush();
    			Serial.println("send reset");
                delay(50);
                serialFlush();
		        this->end();
		        delay(50);

          		this->begin();
			}
		    	
			return false;
		}
		else
		    nJsonErrCount = 0;
		    
		String hdr = doc["hdr"];
        //Serial.println("RECEIVE HDR");
		if (hdr != "DET")
			return false;

		m_id = doc["id"];
		m_type = doc["t"];
		if (m_id <= 0 || m_type < 0)
		    return false;

		m_x = map(doc["x"], 0, 640, 0, 1000);
		m_y = map(doc["y"], 0, 360, 0, 1000);
		m_h = map(doc["h"], 0, 640, 0, 1000);
		m_w = map(doc["w"], 0, 360, 0, 1000);
		return true;
	}
	return false;
}

int SmartSensor::getFuncID()
{
	return m_id;
}

int SmartSensor::getTypeID()
{
	return m_type;
}

int SmartSensor::getPosX()
{
	return m_x;
}

int SmartSensor::getPosY()
{
	return m_y;
}

int SmartSensor::getH()
{
	return m_h;
}

int SmartSensor::getW()
{
	return m_w;
}
