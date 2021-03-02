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

#include "Pixetto.h"

#define PXT_PACKET_START 	0xFD
#define PXT_PACKET_END   	0xFE

#define PXT_CMD_STREAMON	0x79
#define PXT_CMD_STREAMOFF	0x7A
#define PXT_CMD_ENABLEFUNC	0x7D
#define PXT_CMD_DETMODE		0x7E

#define PXT_RET_CAM_SUCCESS	0xE0
#define PXT_RET_CAM_ERROR	0xE1

#define PXT_RET_OBJNUM		0x46 //70

#define PXT_BUF_SIZE		40

#define MAX_OPENCAM_ERROR   5
#define MAX_HEX_ERROR   	30

//#define DEBUG_LOG

template <class SerType>
class InnerSensor
{
public:
	InnerSensor(SerType* p); // SoftwareSerial or HardwareSerial
	
	void enableUVC(bool uvc);
	void begin();
	void end();
	void flush();
	void enableFunc(Pixetto::EFunc fid);
	void setDetectMode(bool mode);
	
	bool isDetected();
	int getFuncID();
	int getTypeID();
	int getPosX();
	int getPosY();
	int getH();
	int getW();
	int numObjects();
	void getLanePoints(int* lx1, int* ly1, int* lx2, int* ly2, int* rx1, int* ry1, int* rx2, int* ry2);
	float getLanesField(Pixetto::ELanesField field);
	void getEquationExpr(char *buf, int len);
	float getEquationAnswer();
	void getApriltagInfo(float* px, float* py, float* pz, int* rx, int* ry, int* rz, int* cx, int* cy);
	float getApriltagField(Pixetto::EApriltagField field);
	
private:
	void resetUboot();
	bool openCam();
	bool readFromSerial();
	void calcDataChecksum(uint8_t *buf, int len);
	bool verifyDataChecksum(uint8_t *buf, int len);
	void parse_Lanes(uint8_t *buf);
	void parse_Equation(uint8_t *buf, int len);
	void parse_Apriltag(uint8_t *buf);
	void parse_SimpleClassifier(uint8_t *buf);
	void sendDetModeCommand();
	void sendQueryCommand();
	void sendFuncCommand();
	void clearDetectedData();
		                     
	bool isCamOpened;
	bool bSendStreamOn;
	bool hasDelayed;
	int  nOpenCamFailCount;
	int  nHexErrCount;
	unsigned long nTime4ObjNum;
	bool bEnableUVC;
	bool bDetMode;  // false: Event mode, true: Callback mode
	bool m_bDetModeDone;
	
	int  m_nFuncID;
	bool m_bFuncDone;
	
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
	nHexErrCount(0), nOpenCamFailCount(0), 
	bEnableUVC(false), bDetMode(false), m_bDetModeDone(false), nTime4ObjNum(0),
	m_dataLen(0), m_nFuncID(0), m_bFuncDone(true),
	m_posx(0.0), m_posy(0.0), m_posz(0.0), m_rotx(0), m_roty(0), m_rotz(0),
	m_centerx(0), m_centery(0)
{
	swSerial = p;
	
	memset(m_inbuf,  0, sizeof(m_inbuf));
	memset(m_points, 0, sizeof(m_points));
	memset(m_eqExpr, 0, sizeof(m_eqExpr));
}

template <class SerType>
void InnerSensor<SerType>::resetUboot()
{
#ifdef DEBUG_LOG
	Serial.println("resetUboot");
#endif	
	// reset to prevent from being blocked in u-boot.
	end();
	delay(50);
	
	swSerial->begin(115200);
	swSerial->print("reset\n");
	swSerial->print("reset\n");
	swSerial->print("reset\n");
	swSerial->flush();
	delay(50);
	flush();
	swSerial->end();
	delay(50);
	
	begin();
}

template <class SerType>
void InnerSensor<SerType>::flush()
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
void InnerSensor<SerType>::setDetectMode(bool mode)
{
	bDetMode = mode;
	m_bDetModeDone = false;
}

template <class SerType>
void InnerSensor<SerType>::begin()
{                            
	swSerial->begin(38400);
	swSerial->setTimeout(50);
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
void InnerSensor<SerType>::clearDetectedData()
{
	m_id = 0; m_type = 0; 
	m_x = 0; m_y = 0; m_h = 0; m_w = 0; 
	m_eqAnswer = 0; m_eqLen = 0;
	m_posx = 0.0; m_posy = 0.0; m_posz = 0.0; m_rotx = 0; m_roty = 0; m_rotz = 0;
	m_centerx = 0; m_centery = 0;
	memset(m_inbuf,  0, sizeof(m_inbuf));
	memset(m_points, 0, sizeof(m_points));
	memset(m_eqExpr, 0, sizeof(m_eqExpr));
}

template <class SerType>
void InnerSensor<SerType>::enableFunc(Pixetto::EFunc fid)
{
	if (fid < 0 || fid > Pixetto::FUNC_VOICE_COMMAND || fid == 5 || fid == 7) 
		return;
	
	m_nFuncID = fid;
	m_bFuncDone = false;
#ifdef DEBUG_LOG
	Serial.print("enableFunc=");
	Serial.println(fid);
#endif	
}

template <class SerType>
void InnerSensor<SerType>::sendDetModeCommand()
{       
	uint8_t SENSOR_CMD[] =  {PXT_PACKET_START, 0x06, PXT_CMD_DETMODE, bDetMode?1:0, 0, PXT_PACKET_END};
	calcDataChecksum(SENSOR_CMD, 6);		
	swSerial->write(SENSOR_CMD, sizeof(SENSOR_CMD)/sizeof(uint8_t));
}

template <class SerType>
void InnerSensor<SerType>::sendQueryCommand()
{
	uint8_t SENSOR_CMD[] = {PXT_PACKET_START, 0x06, PXT_CMD_ENABLEFUNC, m_nFuncID, 0, PXT_PACKET_END};
	calcDataChecksum(SENSOR_CMD, 6);
	swSerial->write(SENSOR_CMD, sizeof(SENSOR_CMD)/sizeof(uint8_t));
}

template <class SerType>
void InnerSensor<SerType>::sendFuncCommand()
{       
	if (!bDetMode && !m_bFuncDone)
	{
		sendQueryCommand();
#ifdef DEBUG_LOG 
		Serial.print("sendFuncCommand=");
		Serial.println(m_nFuncID);
#endif
		m_bFuncDone = true;
	}
}

template <class SerType>
bool InnerSensor<SerType>::openCam()
{
	if (isCamOpened)
		return true;
	
	if (!hasDelayed)
	{
		delay(3000);		
		hasDelayed = true;
	}
	else
		delay(1000);

	if (nOpenCamFailCount > MAX_OPENCAM_ERROR)
	{
		resetUboot();
		bSendStreamOn = false;
		nOpenCamFailCount = 0;
		delay(2000);
	}

	// If it has not received response for the previous streamon cmd yet,
	// do not send streamon command again.
	if (!bSendStreamOn)
	{
		uint8_t SENSOR_CMD[] =  {PXT_PACKET_START, 0x05, PXT_CMD_STREAMOFF, 0, PXT_PACKET_END};
		calcDataChecksum(SENSOR_CMD, 5);
		swSerial->write(SENSOR_CMD, sizeof(SENSOR_CMD)/sizeof(uint8_t));
		delay(500);
		flush();

		SENSOR_CMD[2] = PXT_CMD_STREAMON;
		calcDataChecksum(SENSOR_CMD, 5);
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
#ifdef DEBUG_LOG
			Serial.print(input, HEX);
			Serial.print(" ");
#endif			
			if (input == 0xFF) // no data
			{
				nOpenCamFailCount++;
				return false;
			}                                              
			continue;
		}

		buffer[i++] = input;

#ifdef DEBUG_LOG
        Serial.println("");
#endif
		while ((input = swSerial->read()) != PXT_PACKET_END)
		{
#ifdef DEBUG_LOG
			Serial.print(input, HEX);Serial.print(" ");
#endif
			if (input == 0xFF) // no data 
			{
				delay(1);
				nodata++;
				if (nodata > 10)
				{
					nOpenCamFailCount++;
					return false;
				}
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
				nOpenCamFailCount++;
				return false;
			}
			buffer[i++] = input;
			nodata = 0;
		}	
		buffer[i] = input;

#ifdef DEBUG_LOG
        Serial.println("");
#endif
		if (buffer[2] == PXT_RET_CAM_SUCCESS)
		{
#ifdef DEBUG_LOG
			Serial.println("STREAMON OK!!");
#endif
			isCamOpened = true;
			
			if (!m_bDetModeDone) {
				sendDetModeCommand();
				m_bDetModeDone = true;
			}
			return true;
		}
		else
		{
			nOpenCamFailCount++;
#ifdef DEBUG_LOG
			Serial.println("STREAMON Failed!!");
#endif
		}
	}
	else
	{      
		nOpenCamFailCount++;
#ifdef DEBUG_LOG
		Serial.println("STREAMON no return!!");
#endif
	}
	return false;
}

template <class SerType>
void InnerSensor<SerType>::calcDataChecksum(uint8_t *buf, int len)
{
	uint8_t sum = 0;
	for (int i=1; i<len-2; i++)
		sum += buf[i];
		
	sum %= 256;
	
	buf[len-2] = sum; 
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
void InnerSensor<SerType>::parse_SimpleClassifier(uint8_t *buf) 
{
	m_type = buf[3] * 256 + buf[4];
	m_x = buf[5];
	m_y = buf[6];
	m_w = buf[7];
	m_h = buf[8];
}

template <class SerType>
bool InnerSensor<SerType>::readFromSerial()
{
	uint8_t tmpbuf[PXT_BUF_SIZE];
	int readnum = 0;

	if (bDetMode == true) {
		int loop=0;
		while (swSerial->available() <= 0 && loop < 100000) loop++;
	}
	
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
#ifdef DEBUG_LOG
			Serial.println("ErrCount+1: (i == readnum -1)");
#endif
			return false;
		}
		
			
		int len = tmpbuf[i+1];
		if (len < 0 || len > PXT_BUF_SIZE || len > readnum - i) {
			nHexErrCount++;
#ifdef DEBUG_LOG
			Serial.println("ErrCount+1: (len < 0 || len > PXT_BUF_SIZE...)");
#endif
			return false;
		}

		memset(m_inbuf, 0, sizeof(m_inbuf));
		memcpy(m_inbuf, tmpbuf+i, len);		
		if (verifyDataChecksum(m_inbuf, len))
		{
			nHexErrCount = 0;
			m_dataLen = len;
#ifdef DEBUG_LOG
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
#ifdef DEBUG_LOG
    Serial.println("ErrCount+1: (readFromSerial failed)");
#endif
    nHexErrCount++;
	return false;
}


template <class SerType>
bool InnerSensor<SerType>::isDetected()
{
	if (bEnableUVC)	
	{
		sendDetModeCommand();
	}
	else
	{
		bool ret = openCam();
		if (!ret) 
		{
#ifdef DEBUG_LOG
			Serial.println("openCam() failed");
#endif
			return false;
		}
	}

	//sendFuncCommand();
	if (!bDetMode && !m_bFuncDone)
	{
		sendQueryCommand();
#ifdef DEBUG_LOG 
		Serial.print("sendFuncCommand=");
		Serial.println(m_nFuncID);
#endif
		m_bFuncDone = true;
	}
	
	if (bDetMode == true) {
		flush();
		sendQueryCommand();
#ifdef DEBUG_LOG
		Serial.print("=> send QUERY : func_id=");
		Serial.println(m_nFuncID);
#endif
	}

	clearDetectedData();
		
	if (readFromSerial())
	{
		m_id = m_inbuf[2];
	
		if (m_id <= 0) {
#ifdef DEBUG_LOG
			Serial.println("None is detected!!");
#endif
			return false;
		}
				
		if (m_id == Pixetto::FUNC_LANES_DETECTION)
		{
			parse_Lanes(m_inbuf);
			m_objnum = 1;
		}
		else if (m_id == Pixetto::FUNC_EQUATION_DETECTION)
		{
		 	parse_Equation(m_inbuf, m_dataLen);
		 	m_objnum = 1;
		}
		else if (m_id == Pixetto::FUNC_APRILTAG)
		{
			parse_Apriltag(m_inbuf);
		}
		else if (m_id == Pixetto::FUNC_SIMPLE_CLASSIFIER)
		{
			parse_SimpleClassifier(m_inbuf);
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
		// Received Error
#ifdef DEBUG_LOG
		if (nHexErrCount > 0) {
			Serial.print("Received Error:");
			Serial.println(nHexErrCount);
		}
#endif
		if (nHexErrCount > MAX_HEX_ERROR) {
			nHexErrCount = 0;
			resetUboot();
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
	//if (m_id <= Pixetto::FUNC_LANES_DETECTION && 
	if ((millis() - nTime4ObjNum) > 500)
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

template <class SerType>
float InnerSensor<SerType>::getApriltagField(Pixetto::EApriltagField field)
{
	switch(field)
	{
		case Pixetto::APRILTAG_POS_X:
			return m_posx;
		case Pixetto::APRILTAG_POS_Y:
			return m_posy;
		case Pixetto::APRILTAG_POS_Z:
			return m_posz;
		case Pixetto::APRILTAG_ROT_X:
			return m_rotx;
		case Pixetto::APRILTAG_ROT_Y:
			return m_roty;
		case Pixetto::APRILTAG_ROT_Z:
			return m_rotz;
		case Pixetto::APRILTAG_CENTER_X:
			return m_centerx;
		case Pixetto::APRILTAG_CENTER_Y:
			return m_centery;
		default:
			return 0;
	}
}

template <class SerType>
float InnerSensor<SerType>::getLanesField(Pixetto::ELanesField field)
{
	switch(field)
	{
		case Pixetto::LANES_LX1:
			return m_points[0];
		case Pixetto::LANES_LY1:
			return m_points[1];
		case Pixetto::LANES_LX2:
			return m_points[2];
		case Pixetto::LANES_LY2:
			return m_points[3];
		case Pixetto::LANES_RX1:
			return m_points[4];
		case Pixetto::LANES_RY1:
			return m_points[5];
		case Pixetto::LANES_RX2:
			return m_points[6];
		case Pixetto::LANES_RY2:
			return m_points[7];
		default:
			return 0;
	}
}

#endif
