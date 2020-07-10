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
#ifndef SmartSensor_h
#define SmartSensor_h

#include <InnerSensor.h>
#include <SoftwareSerial.h>

#define SMARTSENSOR_VERSION 1.0.9


class SmartSensor
{
public:
	SmartSensor(int rx, int tx);
	~SmartSensor();
	
	void enableUVC(bool uvc=false);	// Enable UVC mode while connecting to USB and grove simultaneously
	void begin();		// Initialize the SmartSensor
	void end();         // Uninitialize the SmartSensor
	
	bool isDetected();  // Is object detected?
	int getFuncID();    // ID of the detected object
	int getTypeID();    // Type ID (color or shape or...) of the detected object
	
	int getPosX();      // x-coordinate of the upper-left corner of the detected bject
						// For FUNC_LANES_DETECTION case, it's x-coordinate of the center point
	int getPosY();      // y-coordinate of the upper-left corner of the detected object
						// In FUNC_LANES_DETECTION case, it's y-coordinate of the center point
						
	int getH();         // the height of the detected object
	int getW();         // the width of the detected object
	int getHeight();    // the height of the detected object
	int getWidth();     // the width of the detected object
	int	numObjects();	// the number of detected objects
	
	void getLanePoints(int* lx1, int* ly1, int* lx2, int* ly2, int* rx1, int* ry1, int* rx2, int* ry2);
						// lx1, ly1, lx2, ly2 : coordinates of the two end points of the left line
						// rx1, ry1, rx2, ry2 : coordinates of the two end points of the right line

	// For Equation detection, it always returns the first one detected equation.						
	void getEquationExpr(char *buf, int len);	// the detected equation expression, ex."2+3"
	float getEquationAnswer();					// the answer of the equation
	
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
		LETTER_X,
		LETTER_Y,
		LETTER_Z
	};

private:
	SoftwareSerial *swSer;
	InnerSensor<HardwareSerial> *ss_hw;
	InnerSensor<SoftwareSerial> *ss_sw;
	bool m_flag;
};

#endif
