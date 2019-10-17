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
#ifndef SmartSensor_hwserial_h
#define SmartSensor_hwserial_h
#include <SoftwareSerial.h>

#define DINO_VERSION 1.0.1

class SmartSensor
{
	public:
        SmartSensor();
        ~SmartSensor();

        void begin();
        void end();

        bool isDetected();  // 是否偵測到物件
        int getFuncID();    // 偵測物的功能ID
        int getTypeID();    // 該功能有Type才有值 (顏色 or 形狀ID)
        int getPosX();      // 偵測到物件的中心位置x座標
        int getPosY();      // 偵測到物件的中心位置y座標
        int getH();         // 物件高度
        int getW();         // 物件寬度

	private:
	    void serialFlush();
	    bool openCam();
	    bool isCamOpened;
	    bool bSendStreamOn;
	    bool hasDelayed;
	    int  nOpenCamFailCount;
	    int  nJsonErrCount;

	    int m_id;
	    int m_type;
	    int m_x;
	    int m_y;
	    int m_h;
	    int m_w;
};
#endif
