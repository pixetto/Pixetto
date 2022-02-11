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

#define PXT_CMD_GET_VERSION	0xD0
#define PXT_CMD_SET_DETMODE	0xD1
#define PXT_CMD_SET_FUNC    0xD2
#define PXT_CMD_SET_DELAY   0xD4
#define PXT_CMD_GET_DATA    0xD8

#define PXT_RET_FW_VERSION	0xE3

#define PXT_RET_OBJNUM		0x46 //70

#define PXT_BUF_SIZE  		64

#define MAX_HEX_ERROR   	30
#define PXT_TIMEOUT			7000 //(ms)

#define PXT_WAIT_SERIAL     500000 //(loops)

//#define DEBUG_LOG

int g_nVersion[3] = {1,6,0};

template <class SerType>
class InnerSensor
{
public:
	InnerSensor(SerType* p); // SoftwareSerial or HardwareSerial
	
	void begin();
	void end();
	void flush();
	void enableFunc(Pixetto::EFunc fid);
	void setDetectMode(bool mode);
	
	long getVersion();
	bool isDetected();
	int getFuncID();
	int getTypeID();
	int getPosX();
	int getPosY();
	int getH();
	int getW();
	int numObjects();
	int getSequenceID();
	void getLanePoints(int* lx1, int* ly1, int* lx2, int* ly2, int* rx1, int* ry1, int* rx2, int* ry2);
	float getLanesField(Pixetto::ELanesField field);
	void getEquationExpr(char *buf, int len);
	float getEquationAnswer();
	void getApriltagInfo(float* px, float* py, float* pz, int* rx, int* ry, int* rz, int* cx, int* cy);
	float getApriltagField(Pixetto::EApriltagField field);
	bool hasLane();
	bool hasTrafficSign();	

private:
	bool connectSensor(); //openCam();
	void resetSensor();
	bool readFromSerial(bool bConn);
	void calcDataChecksum(uint8_t *buf, int len);
	bool verifyDataChecksum(uint8_t *buf, int len);
	void parse_Version(uint8_t *buf);
	bool parse_Lanes(uint8_t *buf);
	void parse_Equation(uint8_t *buf, int len);
	void parse_Apriltag(uint8_t *buf);
	void parse_SimpleClassifier(uint8_t *buf);
	void parse_LaneAndSign(uint8_t *buf);
	void sendDetModeCmd();
	void sendFuncCmd();
    void sendDelayCmd();
	void sendQueryCmd();
    void checkSendFunc();
    void checkSendDetMode();
    void checkSendDelay();
	void clearDetectedData();
    void setDelayTime(int nTime);
    		                     
	bool isConnected;
	int  nHexErrCount;
	unsigned long nTimeKeepAlive;
	unsigned long nTime4ObjNum;
	bool m_bDetMode;  // true: Stream mode, false: Callback mode
	bool m_bSetDetModeDone;
	int  m_nFuncID;
	bool m_bSetFuncDone;
    int  m_nDelaytime;
    bool m_bSetDelayDone;	
	bool bEnableKeepAlive;
	int  nVersion[4];
	int  nSeqnum;
	bool bFirstCallback;

   	uint8_t m_readbuf[PXT_BUF_SIZE]; // all data in serial buffer
	int  m_readidx; // m_readbuf index
	int  m_readlen; // m_readbuf data length
	uint8_t m_inbuf[PXT_BUF_SIZE]; // one packet
	int  m_dataLen;
			
	SerType *swSerial;
	int m_id;
	int m_type;
	int m_x;
	int m_y;
	int m_h;
	int m_w;
	int m_objnum;
	int m_seqid;
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
InnerSensor<SerType>::InnerSensor(SerType* p) :
	isConnected(false), nHexErrCount(0), nTimeKeepAlive(0), nTime4ObjNum(0),
	m_bDetMode(false), m_bSetDetModeDone(false),
    m_nFuncID(0), m_bSetFuncDone(false), m_nDelaytime(0), m_bSetDelayDone(false),	
	bEnableKeepAlive(false), nSeqnum(0), bFirstCallback(true),
	m_readidx(0), m_readlen(0), m_dataLen(0),
	m_id(0), m_type(0),	m_x(0), m_y(0), m_h(0), m_w(0), m_objnum(0), m_seqid(0),
	m_eqAnswer(0), m_eqLen(0),
	m_posx(0.0), m_posy(0.0), m_posz(0.0), m_rotx(0), m_roty(0), m_rotz(0),
	m_centerx(0), m_centery(0)
{
	swSerial = p;

	memset(nVersion, -1, sizeof(nVersion));
	memset(m_inbuf,  0, sizeof(m_inbuf));
	memset(m_points, 0, sizeof(m_points));
	memset(m_eqExpr, 0, sizeof(m_eqExpr));
}

template <class SerType>
void InnerSensor<SerType>::begin()
{                            
#ifdef DEBUG_LOG
	Serial.println("begin");
#endif	
	swSerial->begin(38400);
	swSerial->setTimeout(50);
	isConnected = false;
	nHexErrCount = 0;
	nTimeKeepAlive = 0;
    nTime4ObjNum = 0;
    m_bSetDetModeDone = false;
    m_bSetFuncDone = true;
    m_bSetDelayDone = false;	
	bEnableKeepAlive = false;
    
    setDelayTime(0);
}

template <class SerType>
void InnerSensor<SerType>::end()
{
	swSerial->end();
}

template <class SerType>
void InnerSensor<SerType>::resetSensor()
{
#ifdef DEBUG_LOG
	Serial.println("resetSensor");
#endif	
	end();
	delay(50);
	begin();
}

template <class SerType>
void InnerSensor<SerType>::flush()
{
	while (swSerial->available() > 0)
		swSerial->read();
}

template <class SerType>
void InnerSensor<SerType>::setDetectMode(bool mode)
{
	m_bDetMode = mode;
	m_bSetDetModeDone = false;
}

template <class SerType>
void InnerSensor<SerType>::enableFunc(Pixetto::EFunc fid)
{
	if (fid < 0 || fid > Pixetto::FUNC_AUTONOMOUS_DRIVING || fid == 5 || fid == 7) 
		return;
	
	m_nFuncID = fid;
	m_bSetFuncDone = false;
}

template <class SerType>
void InnerSensor<SerType>::setDelayTime(int nTime)
{
	m_nDelaytime = nTime;
	m_bSetDelayDone = false;
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
long InnerSensor<SerType>::getVersion()
{
	if (nVersion[1] == -1)
		return -1;

	return (((long)nVersion[1] << 16) + ((long)nVersion[2] << 8) + nVersion[3]);
}

template <class SerType>
void InnerSensor<SerType>::parse_Version(uint8_t *buf)
{
	nVersion[0] = buf[3];
	nVersion[1] = buf[4];
	nVersion[2] = buf[5];
	nVersion[3] = buf[6];
	
#ifdef DEBUG_LOG
	Serial.print("Firmware version: ");
	Serial.print(nVersion[1]); Serial.print(".");
	Serial.print(nVersion[2]); Serial.print(".");
	Serial.println(nVersion[3]);
#endif
	
	if ((nVersion[1] > g_nVersion[0]) or 
		(nVersion[1] == g_nVersion[0] && nVersion[2] > g_nVersion[1]) or
		(nVersion[1] == g_nVersion[0] && nVersion[2] == g_nVersion[1] && nVersion[3] >= g_nVersion[2]))
	{
		bEnableKeepAlive = true;
#ifdef DEBUG_LOG
		Serial.println("Enable KeepAlive");
#endif
	}	
}

template <class SerType>
bool InnerSensor<SerType>::parse_Lanes(uint8_t *buf)
{
    int aa = 0;
    bool valid = false;
    for (aa=3; aa<13; aa++) {
        if (buf[aa] != 0) {
            valid=true;
            break;
        }
    }
    
	m_x = buf[3];
	m_y = buf[4];
	for (aa=0; aa<8; aa++)
		m_points[aa] = buf[aa+5];
        
    return valid;
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
void InnerSensor<SerType>::parse_LaneAndSign(uint8_t *buf)
{	
	// buf[18]  0:null 1:lane 2:sign 3:both
	if (buf[18] == 0 || buf[18] == 2)
	{
		m_x = -1; m_y = -1;
	}
	else
	{
        int aa=0;
        bool valid = false;
        for (aa=3; aa<13; aa++) {
          if (buf[aa] != 0) {
              valid=true;
              break;
            }
        }

        if (!valid) {
            m_x = -1; m_y = -1;
        }
        else {
    	   	m_x = buf[3];
    		m_y = buf[4];
    		for (aa=0; aa<8; aa++)
    			m_points[aa] = buf[aa+5];
        }
	}
	
	if (buf[18] == 0 || buf[18] == 1)
		m_type = -1;
	else
   		m_type = buf[13];
		//m_x = buf[14];
		//m_y = buf[15];
		//m_w = buf[16];
		//m_h = buf[17];
}

template <class SerType>
bool InnerSensor<SerType>::readFromSerial(bool bConn)
{
	if (!bConn && m_bDetMode == false && m_readidx >= m_readlen) {
		flush();
		sendQueryCmd();
		nSeqnum++;
		if (nSeqnum > 10)
			nSeqnum = 0;
#ifdef DEBUG_LOG
		Serial.print("=> send QUERY: FuncID=");
		Serial.println(m_nFuncID);
#endif

		unsigned long loop=0;
		while (swSerial->available() <= 0 && loop < PXT_WAIT_SERIAL) loop++;

#ifdef DEBUG_LOG
		Serial.print(loop); Serial.println("  Query loop end!");
#endif
		
		if (loop >= PXT_WAIT_SERIAL) {
#ifdef DEBUG_LOG
			Serial.println("Query no reponse!");
#endif
			resetSensor();
			return false;
		}
	}
	
	if (m_readidx >= m_readlen && swSerial->available() > 0)
	{
		memset(m_readbuf, 0 ,sizeof(m_readbuf));
		m_readidx = 0;
		m_readlen = 0;
		if ((m_readlen = swSerial->readBytes(m_readbuf, PXT_BUF_SIZE)) != 0) {
#ifdef DEBUG_LOG
			Serial.print("recv: ");
			Serial.println(m_readlen);
			for (int j=0; j<m_readlen; j++)
			{
				Serial.print(m_readbuf[j], HEX); Serial.print(" ");
			}
			Serial.println(" ");
#endif
		}
	}
	
	if (m_readlen == 0)
		return false;
	
	while (m_readidx < m_readlen)
	{
		if (m_readbuf[m_readidx] != PXT_PACKET_START) {
			m_readidx++; continue;
		}
	
		if (m_readidx == m_readlen - 1) {
			nHexErrCount++;
#ifdef DEBUG_LOG
			Serial.println("ErrCount+1: (i == readnum -1)");
#endif
			return false;
		}
		
			
		int len = m_readbuf[m_readidx+1];
		if (len < 0 || len > PXT_BUF_SIZE || len > m_readlen - m_readidx) {
			nHexErrCount++;
#ifdef DEBUG_LOG
			Serial.println("ErrCount+1: (len < 0 || len > PXT_BUF_SIZE...)");
#endif
			flush(); // for long packets, once a broken packet occurs, subsequent packets will be always broken and never recovered until reboot.
       		memset(m_readbuf, 0 ,sizeof(m_readbuf));
			m_readidx = 0;
			m_readlen = 0;

			return false;
		}

		memset(m_inbuf, 0, sizeof(m_inbuf));
		memcpy(m_inbuf, m_readbuf+m_readidx, len);
		if (verifyDataChecksum(m_inbuf, len))
		{
			nHexErrCount = 0;
			m_dataLen = len;
#ifdef DEBUG_LOG
			Serial.print("one packet: ");
			for (int j=0; j<m_dataLen; j++)
			{
				Serial.print(m_inbuf[j], HEX); Serial.print(" ");
			}
			Serial.println(" ");
#endif	

			// if OBJNUM is received, parse one more packet for real object
			if (m_inbuf[2] == PXT_RET_OBJNUM) {
				nTimeKeepAlive = millis();
				if (m_inbuf[3] > 0) { 
				    m_objnum  = m_inbuf[3];
				    nTime4ObjNum = millis();
				}
				m_readidx += m_dataLen;
				continue;
			}
            m_readidx += m_dataLen;
#ifdef DEBUG_LOG
			Serial.print("m_readidx=");
            Serial.println(m_readidx);
			Serial.print("m_readlen=");
            Serial.println(m_readlen);
#endif
			return true;
		}	
		else
		{
			memset(m_inbuf, 0, sizeof(m_inbuf));
			m_readidx++;
#ifdef DEBUG_LOG
			Serial.print("m_readidx=");
            Serial.println(m_readidx);
			Serial.print("m_readlen=");
            Serial.println(m_readlen);
#endif
		}
	}
#ifdef DEBUG_LOG
    Serial.println("ErrCount+1: (readFromSerial failed)");
#endif
    nHexErrCount++;
	return false;
}


template <class SerType>
bool InnerSensor<SerType>::connectSensor()
{
    if (isConnected && bEnableKeepAlive && (millis() - nTimeKeepAlive) > PXT_TIMEOUT) { // camera is not alive.
#ifdef DEBUG_LOG
		Serial.println("camera is not alive!");
#endif
		resetSensor();
	}
    
    if (isConnected)
		return true;
		
    uint8_t SENSOR_CMD[] =  {PXT_PACKET_START, 0x05, PXT_CMD_GET_VERSION, 0, PXT_PACKET_END};
	calcDataChecksum(SENSOR_CMD, 5);
	swSerial->write(SENSOR_CMD, sizeof(SENSOR_CMD)/sizeof(uint8_t));
	flush();
#ifdef DEBUG_LOG
	Serial.println("send: GETVERSION");
#endif

   	unsigned long loop=0;
    while (swSerial->available() <= 0 && loop < PXT_WAIT_SERIAL) loop++;
    
    if (loop >= PXT_WAIT_SERIAL) // camera is not ready.
        return false;
        
    if (readFromSerial(true))
    {
		m_id = m_inbuf[2];
		nTimeKeepAlive = millis();

		if (m_id == (int)PXT_RET_FW_VERSION) {
			parse_Version(m_inbuf);
#ifdef DEBUG_LOG
    		Serial.println("GETVERSION OK!! ");
#endif				
            isConnected = true;
            return true;
		}
	}

	// no packet 
#ifdef DEBUG_LOG
    Serial.println("GETVERSION no return!! ");
#endif				

	return false;
}

template <class SerType>
void InnerSensor<SerType>::sendDetModeCmd()
{       
	uint8_t SENSOR_CMD[] =  {PXT_PACKET_START, 0x06, PXT_CMD_SET_DETMODE, m_bDetMode?(uint8_t)1:(uint8_t)0, 0, PXT_PACKET_END};
	calcDataChecksum(SENSOR_CMD, 6);		
	swSerial->write(SENSOR_CMD, sizeof(SENSOR_CMD)/sizeof(uint8_t));
}

template <class SerType>
void InnerSensor<SerType>::sendFuncCmd()
{
	uint8_t SENSOR_CMD[] = {PXT_PACKET_START, 0x06, PXT_CMD_SET_FUNC, (uint8_t)m_nFuncID, 0, PXT_PACKET_END};
	calcDataChecksum(SENSOR_CMD, 6);
	swSerial->write(SENSOR_CMD, sizeof(SENSOR_CMD)/sizeof(uint8_t));
}

template <class SerType>
void InnerSensor<SerType>::sendQueryCmd()
{
	uint8_t SENSOR_CMD[] = {PXT_PACKET_START, 0x05, PXT_CMD_GET_DATA, 0, PXT_PACKET_END};
	calcDataChecksum(SENSOR_CMD, 5);
	swSerial->write(SENSOR_CMD, sizeof(SENSOR_CMD)/sizeof(uint8_t));
}

template <class SerType>
void InnerSensor<SerType>::sendDelayCmd()
{
	uint8_t SENSOR_CMD[] = {PXT_PACKET_START, 0x06, PXT_CMD_SET_DELAY, (uint8_t)m_nDelaytime, 0, PXT_PACKET_END};
	calcDataChecksum(SENSOR_CMD, 6);
	swSerial->write(SENSOR_CMD, sizeof(SENSOR_CMD)/sizeof(uint8_t));
}

template <class SerType>
void InnerSensor<SerType>::checkSendFunc()
{       
	if (!m_bSetFuncDone)
	{
		sendFuncCmd();
		m_bSetFuncDone = true;
#ifdef DEBUG_LOG 
		Serial.print("sendFuncCmd="); Serial.println(m_nFuncID);
#endif
	}
}

template <class SerType>
void InnerSensor<SerType>::checkSendDetMode()
{       
	if (!m_bSetDetModeDone) {
		sendDetModeCmd();
		m_bSetDetModeDone = true;
#ifdef DEBUG_LOG 
		Serial.print("sendDetMode="); Serial.println(m_bDetMode);
#endif
	}
}

template <class SerType>
void InnerSensor<SerType>::checkSendDelay()
{       
	if (!m_bSetDelayDone)
	{
		sendDelayCmd();
		m_bSetDelayDone = true;
#ifdef DEBUG_LOG
		Serial.print("sendDelayCmd="); Serial.println(m_nDelaytime);
#endif
	}
}

template <class SerType>
bool InnerSensor<SerType>::isDetected()
{
	bool ret = connectSensor();
	if (!ret) {
#ifdef DEBUG_LOG
		Serial.println("connectSensor() failed");
#endif
		return false;
	}

    checkSendDetMode();
	checkSendFunc();
    checkSendDelay();

   	if (m_bDetMode == false && bFirstCallback) {
        bFirstCallback = false;
		flush();
		sendQueryCmd();
        nSeqnum++;

#ifdef DEBUG_LOG
		Serial.print("=> send QUERY: FuncID=");
		Serial.println(m_nFuncID);
#endif
		return false;
	}


	clearDetectedData();
		
	if (readFromSerial(false))
	{
		m_id = m_inbuf[2];
		nTimeKeepAlive = millis();

		if (m_id == 0) {
#ifdef DEBUG_LOG
			Serial.println("None is detected!!");
#endif
			return false;
		}
		if (m_nFuncID > 0 && m_id != m_nFuncID) { // In the case that setfunc has been called, and receive another func result.
#ifdef DEBUG_LOG
			Serial.print("Func ");
			Serial.print(m_nFuncID);
			Serial.println(" is not supported.");
#endif
            sendFuncCmd();	// If setfunc is not supported, harp will load previous function automatically,
							// and it may lead to unexpected detection result. In this case, sendFuncCmd again.
			return false;
		}

		if (m_inbuf[1] == 5) { //empty packet
#ifdef DEBUG_LOG
			Serial.println("None is detected!!");
#endif
			return false;
		}

		if (m_id == Pixetto::FUNC_LANES_DETECTION)	{
			bool ret = parse_Lanes(m_inbuf);
            if (!ret)
                return false;
            else
			 m_objnum = 1;
		}
		else if (m_id == Pixetto::FUNC_EQUATION_DETECTION) {
		 	parse_Equation(m_inbuf, m_dataLen);
		 	m_objnum = 1;
		}
		else if (m_id == Pixetto::FUNC_APRILTAG) {
			parse_Apriltag(m_inbuf);
		}
		else if (m_id == Pixetto::FUNC_NEURAL_NETWORK || m_id == Pixetto::FUNC_SIMPLE_CLASSIFIER) {
			parse_SimpleClassifier(m_inbuf);
		}
		else if (m_id == Pixetto::FUNC_AUTONOMOUS_DRIVING) {
			parse_LaneAndSign(m_inbuf);
			m_objnum = 1;
		}
		else {
			m_type = m_inbuf[3];
			m_x = m_inbuf[4];
			m_y = m_inbuf[5];
			m_w = m_inbuf[6];
			m_h = m_inbuf[7];
		}	
        m_seqid = nSeqnum;

#ifdef DEBUG_LOG
		Serial.print("seqid=");
		Serial.println(m_seqid);
		Serial.println("isDetected() OK!! ");
#endif				
		return true;
	}
	else // no packet 
	{
		/*
    	if (m_bDetMode == false) {
			flush();
			sendQueryCmd();
            nSeqnum++;
			if (nSeqnum > 10)
				nSeqnum = 0;

#ifdef DEBUG_LOG
			Serial.print("=> send QUERY: FuncID=");
			Serial.println(m_nFuncID);
#endif
			return false;
		}
		*/

		// Received Error
#ifdef DEBUG_LOG
		if (nHexErrCount > 0) {
			Serial.print("Received Error:");
			Serial.println(nHexErrCount);
		}
#endif
		if (nHexErrCount > MAX_HEX_ERROR) {
			nHexErrCount = 0;
			resetSensor();
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
	if ((millis() - nTime4ObjNum) > 500)
		m_objnum = 0;
	return m_objnum;    
}

template <class SerType>
int InnerSensor<SerType>::getSequenceID()
{
	return m_seqid;
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
	if (m_eqLen > len)
		m_eqLen = len-1;
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
