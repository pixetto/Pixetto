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
	void getLanePoints(int* lx1, int* ly1, int* lx2, int* ly2, int* rx1, int* ry1, int* rx2, int* ry2);
	void getEquationExpr(char *buf, int len);
	float getEquationAnswer();
	
private:
	void serialFlush();
	bool openCam();
	bool verifyChecksum(uint8_t *buf, int len);
	void parse_Lanes(uint8_t *buf);
	void parse_Equation(uint8_t *buf, int len);
	
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
	int m_points[8];
	char m_eqExpr[17];
	float m_eqAnswer;
	int m_eqLen;
};

#define PXT_PACKET_START 	0xFD
#define PXT_PACKET_END   	0xFE

#define PXT_CMD_STREAMON	0x79
#define PXT_CMD_STREAMOFF	0x7A

#define PXT_RET_CAM_SUCCESS	0xE0
#define PXT_RET_CAM_ERROR	0xE1

#define PXT_RET_OBJNUM		0x46 //70
#define PXT_FUNCID_LANES	16
#define PXT_FUNCID_EQUATION	17

#define PXT_BUF_SIZE		28

#define MAX_OPENCAM_ERROR   7
#define MAX_JSON_ERROR   	30

//#define DEBUG_LOG

template <class SerType>
InnerSensor<SerType>::InnerSensor(SerType* p)
	: m_id(0), m_type(0),
	m_x(0), m_y(0), m_h(0), m_w(0), m_eqAnswer(0), m_eqLen(0),
	isCamOpened(false), hasDelayed(false), bSendStreamOn(false),
	nJsonErrCount(0), nOpenCamFailCount(0), bEnableUVC(false), nTime4ObjNum(0)
{
	swSerial = p;
	
	for (int i=0; i<8; i++)
		m_points[i]=0;
		
	memset(m_eqExpr, 0, 17);
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
		delay(2000);		
		hasDelayed = true;
	}
	else
		delay(1000);

	// If it has not received response for the previous streamon cmd yet,
	// do not send streamon command again.
	if (!bSendStreamOn)
	{
		uint8_t SENSOR_CMD[] =  {PXT_PACKET_START, 0x05, PXT_CMD_STREAMOFF, 0, PXT_PACKET_END};
		swSerial->write(SENSOR_CMD, sizeof(SENSOR_CMD)/sizeof(uint8_t));
		delay(2000);
		serialFlush();

		SENSOR_CMD[2] =  PXT_CMD_STREAMON;
		swSerial->write(SENSOR_CMD, sizeof(SENSOR_CMD)/sizeof(uint8_t));
	
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
	
	sum %= 256;
	
	if (sum == PXT_PACKET_START || sum == PXT_PACKET_END)
		sum = 0xAA;
	
	return (sum == buf[len-2]);
}

template <class SerType>
void InnerSensor<SerType>::parse_Lanes(uint8_t *buf)
{
	m_x = buf[3];
	m_y = buf[4];
	for (int aa=0; aa<8; aa++)
		m_points[aa] = buf[aa+5];
}

template <class SerType>
void InnerSensor<SerType>::parse_Equation(uint8_t *buf, int len)
{
	m_x = buf[3];
	m_y = buf[4];
	m_w = buf[5];
	m_h = buf[6];
	
	m_eqAnswer = buf[7]*256 + buf[8] + (float)buf[9]/100.0;
	m_eqLen = len - 12;
	for (int aa=0; aa<m_eqLen; aa++)
		m_eqExpr[aa] = (char)buf[aa+10];	
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
		uint8_t buffer[PXT_BUF_SIZE] = {0};
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
			if (i >= PXT_BUF_SIZE)
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
		
		if (buffer[1] <= 28) // Length <= 28 
		{
			buffer[i] = input;
	
#ifdef DEBUG_LOG
			Serial.print("recv: ");
			for (int j=0; j<PXT_BUF_SIZE; j++)
			{
				Serial.print(buffer[j], HEX);
				Serial.print(" ");
			}
			Serial.println(" ");
#endif		
			correct = verifyChecksum(buffer, buffer[1]);
			if (correct)
			{
				nJsonErrCount = 0;
				
				m_id = buffer[2];
	
				if (m_id <= 0)
				{
#ifdef DEBUG_LOG
					Serial.println("  incorrect values!!");
#endif
					return false;
				}
				
				if (m_id == PXT_FUNCID_LANES)
				{
					parse_Lanes(buffer);
					m_objnum = 1;
				}
				else if (m_id == PXT_FUNCID_EQUATION)
				{
				 	parse_Equation(buffer, i+1);
				 	m_objnum = 1;
				}
				else
				{
					m_type = buffer[3];
	
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
					m_x = buffer[4];
					m_y = buffer[5];
					m_w = buffer[6];
					m_h = buffer[7];
				}	
#ifdef DEBUG_LOG
				Serial.println("  OK!! ");
#endif				
				return true;
			}
			else // validate checksum failed
			{
#ifdef DEBUG_LOG
				Serial.println("  WRONG!! ");
#endif			
		    }
		} // buffer[1] (Length) <= 28
		
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
	if (m_id <= PXT_FUNCID_LANES && (millis() - nTime4ObjNum) > 1000)
		m_objnum = 0;
	return m_objnum;    
}

template <class SerType>
void InnerSensor<SerType>::getLanePoints(int* lx1, int* ly1, int* lx2, int* ly2, int* rx1, int* ry1, int* rx2, int* ry2)
{
	if (!lx1 || !ly1 || !lx2 || !ly2 || !rx1 || !ry1 || !rx2 || !ry2)
		return;
		
	*lx1 = m_points[0];
	*ly1 = m_points[1];
	*lx2 = m_points[2];
	*ly2 = m_points[3];
	*rx1 = m_points[4];
	*ry1 = m_points[5];
	*rx2 = m_points[6];
	*ry2 = m_points[7];
}

template <class SerType>
void InnerSensor<SerType>::getEquationExpr(char *buf, int len)
{
	 strncpy(buf, m_eqExpr, m_eqLen);
	 buf[m_eqLen] = '\0';
}

template <class SerType>
float InnerSensor<SerType>::getEquationAnswer()
{
	return m_eqAnswer;
}

#endif
