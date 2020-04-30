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

template <class SerType>
class InnerSensor
{
public:
	InnerSensor(SerType* p); // SoftwareSerial or HardwareSerial
	~InnerSensor();
	
	void enableUVC(bool uvc);
	void begin();
	void end();
	
	bool isDetected();
	int getFuncID();
	int getTypeID();
	int getPosX();
	int getPosY();
	int getH();
	int getW();
	int numObjects();
	
private:
	void serialFlush();
	bool openCam();
	bool verifyChecksum(uint8_t *buf, int len);
	
	bool isCamOpened;
	bool bSendStreamOn;
	bool hasDelayed;
	int  nOpenCamFailCount;
	int  nJsonErrCount;
	bool bEnableUVC;
	unsigned long nTime4ObjNum;
	
	SerType *swSerial;
	int m_id;
	int m_type;
	int m_x;
	int m_y;
	int m_h;
	int m_w;
	int m_objnum;
};

#define PXT_PACKET_START 	0xFD
#define PXT_PACKET_END   	0xFE

#define PXT_CMD_STREAMON	0x79

#define PXT_RET_CAM_SUCCESS	0xE0
#define PXT_RET_CAM_ERROR	0xE1

#define PXT_RET_OBJNUM		0x46 //70

//#define SENSOR_CMD_STREAMON  "{\"header\":\"STREAMON\"};"
//#define SENSOR_CMD_QUERY	 "{\"header\":\"QUERY\"};"
const uint8_t SENSOR_CMD_STREAMON[] =  {PXT_PACKET_START, 0x05, PXT_CMD_STREAMON, 0, PXT_PACKET_END};

#define MAX_OPENCAM_ERROR   7
#define MAX_JSON_ERROR   	30

//#define DEBUG_LOG

template <class SerType>
InnerSensor<SerType>::InnerSensor(SerType* p)
	: m_id(0), m_type(0),
	m_x(0), m_y(0), m_h(0), m_w(0),
	isCamOpened(false), hasDelayed(false), bSendStreamOn(false),
	nJsonErrCount(0), nOpenCamFailCount(0), bEnableUVC(false), nTime4ObjNum(0)
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
void InnerSensor<SerType>::enableUVC(bool uvc)
{
	bEnableUVC = uvc;	                            
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
		delay(5000); // depends on pixetto start-up time
		hasDelayed = true;
	}
	else
		delay(1000);

	// If it has not received response for the previous streamon cmd yet,
	// do not send streamon command again.
	if (!bSendStreamOn)
	{
		serialFlush();
		swSerial->write(SENSOR_CMD_STREAMON, sizeof(SENSOR_CMD_STREAMON)/sizeof(uint8_t));
	
#ifdef DEBUG_LOG
		Serial.println("send: STREAMON");
#endif
		bSendStreamOn = true;
	}

	if (swSerial->available() > 0)
	{
		bSendStreamOn = false; // After receiving any response, reset the flag
	
		uint8_t buffer[10];
		uint8_t input;
		int i=0;
		int nodata=0;
		
		while ((input = swSerial->read()) != PXT_PACKET_START)
		{
			if (input == 0xFF) // no data
				return false;

			continue;
		}

		buffer[i++] = input;
		 		
		while ((input = swSerial->read()) != PXT_PACKET_END)
		{
			if (input == 0xFF) // no data 
			{
				delay(1);
				nodata++;
				if (nodata > 10)
					return false;
				else
					continue;
			}

			if (input == PXT_PACKET_START)
				i = 0;

			if (i >= 4)
			{
#ifdef DEBUG_LOG
				Serial.println("");
				Serial.println("STREAMON CMD error!!");
#endif
				return false;
			}
			buffer[i++] = input;
			nodata = 0;
		}	
		buffer[i] = input;

		if (buffer[2] == PXT_RET_CAM_SUCCESS)
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
bool InnerSensor<SerType>::verifyChecksum(uint8_t *buf, int len)
{
	uint8_t sum = 0;
	
	for (uint8_t i=1; i<len-2; i++)
		sum += buf[i];
	
	if (sum == PXT_PACKET_START || sum == PXT_PACKET_END)
		sum = 0xAA;
	
	return (sum == buf[8]);
}


template <class SerType>
bool InnerSensor<SerType>::isDetected()
{
	if (!bEnableUVC)
	{
		bool ret = openCam();
		if (!ret)
		{
#ifdef DEBUG_LOG
			Serial.println("openCam() failed");
#endif
			return false;
		}
#ifdef DEBUG_LOG
		//else
		//	Serial.println("openCam() OK");
#endif
	}
	

	if (swSerial->available() > 0)
	{
		uint8_t buffer[10];
		uint8_t input;
		int i=0;
		int nodata=0;
		bool correct=false;
		
		while ((input = swSerial->read()) != PXT_PACKET_START)
		{
			if (input == 0xFF) // no data
				return false;
		}
		buffer[i++] = input;
		
		while ((input = swSerial->read()) != PXT_PACKET_END)
		{
			if (i >= 10)
				break;

			if (input == 0xFF) // no data
			{
				delay(1);
				nodata++;
				if (nodata > 10)
					return false;
				else
					continue;
			}
				
			if (input == PXT_PACKET_START)
				i = 0;

			buffer[i++] = input;
			nodata = 0;
		}
		
		if (i == 9) 
		{
			buffer[i] = input;
	
#ifdef DEBUG_LOG
			Serial.print("recv: ");
			for (int j=0; j<10; j++)
			{
				Serial.print(buffer[j], HEX);
				Serial.print(" ");
			}
#endif		
		    correct = verifyChecksum(buffer, 10);
			if (correct)
			{
				nJsonErrCount = 0;
				
				m_id = buffer[2];
				m_type = buffer[3];

				if (m_id <= 0 || m_type < 0)
				{
#ifdef DEBUG_LOG
					Serial.println("  incorrect values!!");
#endif
					return false;
				}


   				if (m_id == PXT_RET_OBJNUM) {
   					if (m_type > 0) { 
   					    m_objnum  = m_type;
   					    nTime4ObjNum = millis();
   					}
   					//else if (m_type == 0 && (millis() - nTime4ObjNum) > 1000) {
					//	m_objnum  = m_type;
					//}
					return isDetected();
				}
#ifdef DEBUG_LOG
				Serial.println("  OK!! ");
#endif				
				m_x = buffer[4];
				m_y = buffer[5];
				m_w = buffer[6];
				m_h = buffer[7];
		
				return true;
			}
			else // validate checksum failed
			{
#ifdef DEBUG_LOG
				Serial.println("  WRONG!! ");
#endif			
		    }
		}
		
		// Received Error
		nJsonErrCount++;
#ifdef DEBUG_LOG
		Serial.print("Received Error:");
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

template <class SerType>
int InnerSensor<SerType>::getH()
{
	return m_h;
}

template <class SerType>
int InnerSensor<SerType>::getW()
{
	return m_w;
}

template <class SerType>
int InnerSensor<SerType>::numObjects()
{
	if ((millis() - nTime4ObjNum) > 1000)
		m_objnum = 0;
	return m_objnum;    
}

#endif
