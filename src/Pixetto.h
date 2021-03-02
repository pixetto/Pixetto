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
#ifndef Pixetto_h
#define Pixetto_h

#include <Arduino.h>
#include <SoftwareSerial.h>

#define PIXETTO_VERSION 1.5.1

template <class SerType>
class InnerSensor;

class Pixetto
{
public:
	Pixetto(int rx, int tx);
	
	enum EFunc
	{
		FUNC_COLOR_DETECTION		= 1,
		FUNC_COLOR_CODE_DETECTION	= 2,
		FUNC_SHAPE_DETECTION		= 3,
		FUNC_SPHERE_DETECTION		= 4,
		FUNC_TEMPLATE_MATCHING		= 6,
		FUNC_KEYPOINTS				= 8,
		FUNC_NEURAL_NETWORK			= 9,
		FUNC_APRILTAG				= 10,
		FUNC_FACE_DETECTION			= 11,
		FUNC_TRAFFIC_SIGN_DETECTION	= 12,
		FUNC_HANDWRITTEN_DIGITS_DETECTION	= 13,
		FUNC_HANDWRITTEN_LETTERS_DETECTION	= 14,
		FUNC_CLOUD_DETECTION		= 15,
		FUNC_LANES_DETECTION		= 16,
		FUNC_EQUATION_DETECTION		= 17,
		FUNC_SIMPLE_CLASSIFIER		= 18,
		FUNC_VOICE_COMMAND			= 19
	};
	
	enum EColor
	{
		COLOR_RED = 1,
		COLOR_YELLOW,
		COLOR_GREEN,
		COLOR_BLUE,
		COLOR_PURPLE,
		COLOR_BLACK
	};
	
	enum EShape
	{
		SHAPE_ROUND = 1,
		SHAPE_RECTANGLE,
		SHAPE_TRIANGLE,
		SHAPE_PENTAGON
	};
	
	enum ETrafficSign
	{
		SIGN_NO_ENTRE = 0,
		SIGN_NO_LEFT_TURN,
		SIGN_NO_RIGHT_TURN,
		SIGN_WRONG_WAY,
		SIGN_NO_U_TURN,
		SIGN_MAX_SPEED,
		SIGN_ONEWAY_TRAFFIC,
		SIGN_LEFT_TURN,
		SIGN_RIGHT_TURN,
		SIGN_MIN_SPEED,
		SIGN_U_TURN,
		SIGN_TUNNEL_AHEAD,
		SIGN_BEWARE_OF_CHILDREN,
		SIGN_ROUNDABOUT,
		SIGN_YIELD_TO_PEDESTRIAN,
		SIGN_RED_LIGHT,
		SIGN_GREEN_LIGHT
	};

	enum ELetters
	{
		LETTER_A=0,
		LETTER_B,
		LETTER_C,
		LETTER_D,
		LETTER_E,
		LETTER_F,
		LETTER_G,
		LETTER_H,
		LETTER_I,
		LETTER_J,
		LETTER_K,
		LETTER_L,
		LETTER_M,
		LETTER_N,
		LETTER_O,
		LETTER_P,
		LETTER_Q,
		LETTER_R,
		LETTER_S,
		LETTER_T,
		LETTER_U,
		LETTER_V,
		LETTER_W,
		LETTER_X,
		LETTER_Y,
		LETTER_Z
	};

	enum EVoiceCommand
	{
		VOICE_Hello = 1,	// 你好
		VOICE_Thanks,       // 謝謝
		VOICE_Bye,			// 再見
		VOICE_WhatsThis,	// 這是甚麼
		VOICE_WhatTime,		// 現在幾點
		VOICE_HowOld,		// 我幾歲
		VOICE_WhatDay,		// 今天星期幾
		VOICE_TellStory,	// 講故事
		VOICE_TellJoke,		// 說笑話
		VOICE_ReadPoem,		// 念唐詩
		VOICE_TurnOnLight,	// 開燈
		VOICE_TurnOffLight,	// 關燈
		VOICE_TurnLeft,		// 左轉
		VOICE_TurnRight,	// 右轉
		VOICE_GoAhead,		// 前進
		VOICE_MoveBack,		// 後退
		VOICE_Stop,			// 停止
		VOICE_Open,			// 開啟
		VOICE_Close,		// 關閉
		VOICE_OpenEyes1,	// 睜開眼睛
		VOICE_OpenEyes2,	// 睜眼
		VOICE_CloseEyes1,	// 閉上眼睛
		VOICE_CloseEyes2,	// 閉眼
		VOICE_Jump,			// 跳一下
		VOICE_StandUp,		// 起立
		VOICE_SquatDown		// 蹲下		
	};


	enum EApriltagField 
	{
		APRILTAG_POS_X=1,
		APRILTAG_POS_Y,
		APRILTAG_POS_Z,
		APRILTAG_ROT_X,
		APRILTAG_ROT_Y,
		APRILTAG_ROT_Z,
		APRILTAG_CENTER_X,
		APRILTAG_CENTER_Y
	};

	enum ELanesField 
	{
	    LANES_LX1=1,
	    LANES_LY1,
	    LANES_LX2,
	    LANES_LY2,
	    LANES_RX1,
	    LANES_RY1,
	    LANES_RX2,
	    LANES_RY2
	};

	void enableUVC(bool uvc=false);	// Enable UVC mode while connecting to USB and grove simultaneously
	void begin();		// Initialize the Pixetto
	void end();         // Uninitialize the Pixetto
	void flush();		// Clear the serial buffer.
	void enableFunc(Pixetto::EFunc fid); // Switch among functions.

	void setDetectMode(bool mode=false); // Set detect mode (false:event mode, true:callback mode)
	                                     // Attention: the limit of callback mode:
	                                     //   - Neural Network function does not work when object detection algorithm 
										 //     is set to "Central", Blob Detection" and "Blob Detection (Kalman Filter)".
										 //   - Voice Command function does not work on callback mode.
										     
	
	bool isDetected();  // Is there any object detected?
	int getFuncID();    // ID of the detected object
	int getTypeID();    // Type ID (color or shape or...) of the detected object
	
	int getPosX();      // x-coordinate of the upper-left corner of the detected object (range:0~100)
						// In FUNC_LANES_DETECTION case, it's x-coordinate of the center point
	int getPosY();      // y-coordinate of the upper-left corner of the detected object (range:0~100)
						// In FUNC_LANES_DETECTION case, it's y-coordinate of the center point
						
	int getH();         // the height of the detected object (range:0~100)
	int getW();         // the width of the detected object  (range:0~100)
	int getHeight();    // the height of the detected object (range:0~100)
	int getWidth();     // the width of the detected object  (range:0~100)
	int	numObjects();	// the number of detected objects
	
	// For Lanes detection, 
	//   get center point : getPosX(), getPosY()
	//   get 4 end points of left and right lines : getLanePoints()
	//   lx1, ly1, lx2, ly2 : coordinates of the two end points of the left line
	//   rx1, ry1, rx2, ry2 : coordinates of the two end points of the right line
	void getLanePoints(int* lx1, int* ly1, int* lx2, int* ly2, int* rx1, int* ry1, int* rx2, int* ry2);
						
	// For Lanes detection, another function to get the 4 end points.
	//   Get the value by given its corresponding field-id
	//   ex. rx1 = getLanesField(Pixetto::LANES_LX1);
	float getLanesField(Pixetto::ELanesField field);
	
	// For Apriltag detection
	//   px,py,pz : distance to center on each x,y,z axis 
	//   rx,ry,rz : rotation angle on each x,y,z axis
	//   cx,cy    : coordinate of apriltag's center point
	void getApriltagInfo(float* px, float* py, float* pz, int* rx, int* ry, int* rz, int* cx, int* cy);
						
	// For Apriltag detection, another function to get the 8 values of apriltag.
	//   Get the value by given its corresponding field-id
	//   ex. px = getApriltagField(Pixetto::APRILTAG_POS_X);
	float getApriltagField(Pixetto::EApriltagField field);
	
	// For Equation detection, it always returns the first one detected equation.
	void getEquationExpr(char *buf, int len);	// the detected equation expression, ex."2+3"
	float getEquationAnswer();					// the answer of the equation
       
private:
	SoftwareSerial *swSer;
	InnerSensor<HardwareSerial> *ss_hw;
	InnerSensor<SoftwareSerial> *ss_sw;
	bool m_flag;
	int m_rx;
	int m_tx;
};

#endif
