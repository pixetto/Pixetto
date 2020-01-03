/*
 * Copyright 2020 VIA Technologies, Inc. All Rights Reserved.
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
#ifndef InnerSensor_h
#define InnerSensor_h

#include <ArduinoJson.h>


template <class SerType>
class InnerSensor
{
	public:
        InnerSensor(SerType* p); // SoftwareSerial or HardwareSerial
        ~InnerSensor();

        void begin();
        void end();

        bool isDetected();
        int getFuncID();
        int getTypeID();
        int getPosX();
        int getPosY();
        int getH();
        int getW();

	private:
	    void serialFlush();
	    bool openCam();
	    bool isCamOpened;
	    bool bSendStreamOn;
	    bool hasDelayed;
	    int  nOpenCamFailCount;
	    int  nJsonErrCount;

	    SerType *swSerial;
	    int m_id;
	    int m_type;
	    int m_x;
	    int m_y;
	    int m_h;
	    int m_w;
};


#define SENSOR_CMD_STREAMON  "{\"header\":\"STREAMON\"};"
//#define SENSOR_CMD_QUERY	 "{\"header\":\"QUERY\"};"

#define MAX_OPENCAM_ERROR   7
#define MAX_JSON_ERROR   	7

//#define DEBUG_LOG

template <class SerType>
InnerSensor<SerType>::InnerSensor(SerType* p)
 : m_id(0), m_type(0),
   m_x(0), m_y(0), m_h(0), m_w(0),
   isCamOpened(false), hasDelayed(false), bSendStreamOn(false),
   nJsonErrCount(0), nOpenCamFailCount(0)
{
    swSerial = p;
}

template <class SerType>
InnerSensor<SerType>::~InnerSensor()
{
	 if (swSerial)
	    delete swSerial;
}

template <class SerType>
void InnerSensor<SerType>::serialFlush()
{
	while (swSerial->available() > 0)
	    char t = swSerial->read();
}

template <class SerType>
void InnerSensor<SerType>::begin()
{
    swSerial->begin(38400);
    hasDelayed = false;
    isCamOpened = false;
    bSendStreamOn = false;
    nOpenCamFailCount = 0;
    nJsonErrCount = 0;
}

template <class SerType>
void InnerSensor<SerType>::end()
{
    swSerial->end();
}

template <class SerType>
bool InnerSensor<SerType>::openCam()
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

#ifdef DEBUG_LOG
        Serial.println("send: STREAMON");
#endif
		bSendStreamOn = true;
 	}

    if (swSerial->available() > 0)
    {
        bSendStreamOn = false; // After receiving response, reset the flag

        DynamicJsonDocument doc(256);
		String s = swSerial->readStringUntil(';');
#ifdef DEBUG_LOG
		//Serial.print("recv:");
		Serial.println(s);
#endif
		DeserializationError error = deserializeJson(doc, s);
		if (error)
		{
#ifdef DEBUG_LOG
		    Serial.println("STREAMON CMD error!!");
#endif
		    return false;
		}

		String hdr = doc["header"];
		if (hdr == "CAM_SUCCESS")
		{
#ifdef DEBUG_LOG
		    Serial.println("STREAMON OK!!");
#endif
		    isCamOpened = true;
		    return true;
 		}
		else
		{
#ifdef DEBUG_LOG
		    Serial.println("STREAMON Failed!!");
#endif
		}
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

template <class SerType>
bool InnerSensor<SerType>::isDetected()
{
	bool ret = openCam();
    if (!ret)
    {
#ifdef DEBUG_LOG
   	    Serial.println("openCam() failed");
#endif
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
#ifdef DEBUG_LOG
		Serial.println(s);
#endif

		DeserializationError error = deserializeJson(doc, s);
		if (error)
		{
		    nJsonErrCount++;
#ifdef DEBUG_LOG
		    Serial.print("JSON Error:");
		    Serial.println(nJsonErrCount);
#endif
		    if (nJsonErrCount > MAX_JSON_ERROR)
		    {
		        this->end();
		        delay(50);
		        swSerial->begin(115200);
		    	swSerial->print("reset\n");
		    	swSerial->print("reset\n");
		    	swSerial->print("reset\n");
		    	swSerial->flush();
#ifdef DEBUG_LOG
    			Serial.println("send reset");
#endif
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

template <class SerType>
int InnerSensor<SerType>::getFuncID()
{
	return m_id;
}

template <class SerType>
int InnerSensor<SerType>::getTypeID()
{
	return m_type;
}

template <class SerType>
int InnerSensor<SerType>::getPosX()
{
	return m_x;
}

template <class SerType>
int InnerSensor<SerType>::getPosY()
{
	return m_y;
}

template <class T>
int InnerSensor<T>::getH()
{
	return m_h;
}

template <class SerType>
int InnerSensor<SerType>::getW()
{
	return m_w;
}

#endif
