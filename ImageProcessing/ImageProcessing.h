/*
 * Copyright 2019 VIA Technologies, Inc. All Rights Reserved.
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
#include "Wire.h"

#define BUFLEN 128
#define SYN 0x16
#define STX 0x02
#define ETX 0x03

#define MODE_COLOR    0
#define MODE_SHAPE    1
#define MODE_FACE     2
#define MODE_LETTERS  3
#define MODE_DIGITS   4

class ImageProcessing
{
public:
	ImageProcessing();
	~ImageProcessing();
	static uint8_t i2c_addr() { return 0x31; };
	void init();
	bool detect(int mode);
	int getNumOfResult();
	int getIndexOfResult(int num);

private:
	int read(void* buf, size_t count);
	int write(const void* buf, size_t count);
	int recv(void* buf, size_t count);
	int send(const void* buf, size_t count);
	uint8_t readb();
	int sendmsg(const char* msg);
	bool poll();
	
	int result[128];
	int num_result;
};
