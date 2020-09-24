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


#define PXT_PACKET_START 	0xFD
#define PXT_PACKET_END   	0xFE

#define PXT_CMD_STREAMON	0x79
#define PXT_CMD_STREAMOFF	0x7A

#define PXT_RET_CAM_SUCCESS	0xE0
#define PXT_RET_CAM_ERROR	0xE1

#define PXT_RET_OBJNUM		0x46 //70
#define PXT_FUNCID_APRILTAG	10
#define PXT_FUNCID_LANES	16
#define PXT_FUNCID_EQUATION	17

#define PXT_BUF_SIZE		40

#define MAX_OPENCAM_ERROR   7
#define MAX_HEX_ERROR   	30

//#define DEBUG_LOG


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
	void getApriltagInfo(float* px, float* py, float* pz, int* rx, int* ry, int* rz, int* cx, int* cy);
	
private:
	void serialFlush();
	bool openCam();
	bool readFromSerial();
	bool verifyDataChecksum(uint8_t *buf, int len);
	void parse_Lanes(uint8_t *buf);
	void parse_Equation(uint8_t *buf, int len);
	void parse_Apriltag(uint8_t *buf);
	
	bool isCamOpened;
	bool bSendStreamOn;
	bool hasDelayed;
	int  nOpenCamFailCount;
	int  nHexErrCount;
	bool bEnableUVC;
	unsigned long nTime4ObjNum;

	uint8_t m_inbuf[PXT_BUF_SIZE];
	int m_dataLen;
			
	SerType *swSerial;
	int m_id;
	int m_type;
	int m_x;
	int m_y;
	int m_h;
	int m_w;
	int m_objnum;
	// Lanes
	int m_points[8];
	// Equations
	char m_eqExpr[17];
	float m_eqAnswer;
	int m_eqLen;
	// Apriltag
	float m_posx;
	float m_posy;
	float m_posz;
	int m_rotx;
	int m_roty;
	int m_rotz;
	int m_centerx;
	int m_centery;	
};

template <class SerType>
InnerSensor<SerType>::InnerSensor(SerType* p)
	: m_id(0), m_type(0),
	m_x(0), m_y(0), m_h(0), m_w(0), m_eqAnswer(0), m_eqLen(0),
	isCamOpened(false), hasDelayed(false), bSendStreamOn(false),
	nHexErrCount(0), nOpenCamFailCount(0), bEnableUVC(false), nTime4ObjNum(0),
	m_dataLen(0), 
	m_posx(0.0), m_posy(0.0), m_posz(0.0), m_rotx(0), m_roty(0), m_rotz(0),
	m_centerx(0), m_centery(0)
{
	swSerial = p;
	
	memset(m_inbuf,  0, sizeof(m_inbuf));
	memset(m_points, 0, sizeof(m_points));
	memset(m_eqExpr, 0, sizeof(m_eqExpr));
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
	nHexErrCount = 0; 
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
bool InnerSensor<SerType>::verifyDataChecksum(uint8_t *buf, int len)
{
	uint8_t sum = 0;
	
	for (uint8_t i=1; i<len-2; i++)
		sum += buf[i];
	
	sum %= 256;
	
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
	
	m_eqAnswer = 0;
	for (int i=8; i<=14; i++) 
		m_eqAnswer = m_eqAnswer * 10 + buf[i];

	m_eqAnswer /= 100;
	if (buf[7] == 0) m_eqAnswer = 0 - m_eqAnswer;	
	
	memset(m_eqExpr, 0, sizeof(m_eqExpr));
	m_eqLen = len - 17;
	for (int aa=0; aa<m_eqLen; aa++)
		m_eqExpr[aa] = (char)buf[aa+15];
}

template <class SerType>
void InnerSensor<SerType>::parse_Apriltag(uint8_t *buf)
{
	m_type = buf[3];
	m_x = buf[4];
	m_y = buf[5];
	m_w = buf[6];
	m_h = buf[7];

	int value = 0;
	value = (short)(buf[8] * 256 + buf[9]);
	m_posx = (float)value / 100.0;
	value = (short)(buf[10] * 256 + buf[11]);
	m_posy = (float)value / 100.0;
	value = (short)(buf[12] * 256 + buf[13]);
	m_posz = (float)value / 100.0;
	
	m_rotx = (short)(buf[14] * 256 + buf[15]);
	m_roty = (short)(buf[16] * 256 + buf[17]);
	m_rotz = (short)(buf[18] * 256 + buf[19]);
	
	m_centerx = (short)(buf[20] * 256 + buf[21]);
	m_centery = (short)(buf[22] * 256 + buf[23]);
}

template <class SerType>
bool InnerSensor<SerType>::readFromSerial()
{
	uint8_t tmpbuf[PXT_BUF_SIZE];
	int readnum = 0;
	
	if (swSerial->available() > 0)
	{
		memset(tmpbuf, 0 ,sizeof(tmpbuf));
		if ((readnum = swSerial->readBytes(tmpbuf, PXT_BUF_SIZE)) != 0) {
#ifdef DEBUG_LOG
			Serial.print("recv: ");
			Serial.println(readnum);
			for (int j=0; j<readnum; j++)
			{
				Serial.print(tmpbuf[j], HEX);
				Serial.print(" ");
			}
			Serial.println(" ");
#endif
		}
	}
	
	if (readnum == 0)
		return false;
	
	int i = 0;
	while (i < readnum) 
	{
		if (tmpbuf[i] != PXT_PACKET_START) {
			i++;
			continue;
		}
	
		if (i == readnum - 1) {
			nHexErrCount++;
			return false;
		}
		
			
		int len = tmpbuf[i+1];
		if (len < 0 || len > PXT_BUF_SIZE || len > readnum - i) {
			nHexErrCount++;
			return false;
		}
           
		memset(m_inbuf, 0, sizeof(m_inbuf));
		memcpy(m_inbuf, tmpbuf+i, len);		
		if (verifyDataChecksum(m_inbuf, len))
		{
			nHexErrCount = 0;
			m_dataLen = len;
#ifdef DEBUG_LOG
			Serial.println("Read one packet!!");
			Serial.print("one packet: ");
			for (int j=0; j<m_dataLen; j++)
			{
				Serial.print(m_inbuf[j], HEX);
				Serial.print(" ");
			}
			Serial.println(" ");
#endif	

			// if OBJNUM is received, parse one more packet for real object
			if (m_inbuf[2] == PXT_RET_OBJNUM) {
				if (m_inbuf[3] > 0) { 
				    m_objnum  = m_inbuf[3];
				    nTime4ObjNum = millis();
				}
				i += m_dataLen;
				continue;
			}

			return true;
		}	
		else
		{
			memset(m_inbuf, 0, sizeof(m_inbuf));
			i++;
		}
	}
    //Serial.println("Nothing found!!");
    nHexErrCount++;
	return false;
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
	
	if (readFromSerial())
	{
		m_id = m_inbuf[2];
	
		if (m_id <= 0) {
#ifdef DEBUG_LOG
			Serial.println("  incorrect FuncID!!");
#endif
			return false;
		}
				
		if (m_id == PXT_FUNCID_LANES)
		{
			parse_Lanes(m_inbuf);
			m_objnum = 1;
		}
		else if (m_id == PXT_FUNCID_EQUATION)
		{
		 	parse_Equation(m_inbuf, m_dataLen);
		 	m_objnum = 1;
		}
		else if (m_id == PXT_FUNCID_APRILTAG)
		{
			parse_Apriltag(m_inbuf);
		}
		else
		{
			m_type = m_inbuf[3];

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
			m_x = m_inbuf[4];
			m_y = m_inbuf[5];
			m_w = m_inbuf[6];
			m_h = m_inbuf[7];
		}	
#ifdef DEBUG_LOG
		Serial.println("  OK!! ");
#endif				
		return true;
	}
	else // no packet 
	{
#ifdef DEBUG_LOG
		//Serial.println("  WRONG!! ");
#endif			
		// Received Error
#ifdef DEBUG_LOG
		Serial.print("Received Error:");
		Serial.println(nHexErrCount);
#endif
		if (nHexErrCount > MAX_HEX_ERROR)
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

template <class SerType>
void InnerSensor<SerType>::getApriltagInfo(float* px, float* py, float* pz, int* rx, int* ry, int* rz, int* cx, int* cy)
{
	if (!px || !py || !px || !rx || !ry || !rz || !cx || !cy)
		return;
		
	*px = m_posx;
	*py = m_posy;
	*pz = m_posz;
	*rx = m_rotx;
	*ry = m_roty;
	*rz = m_rotz;
	*cx = m_centerx;
	*cy = m_centery;
}


#endif
