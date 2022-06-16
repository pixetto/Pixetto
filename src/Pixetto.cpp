/*
 * Copyright 2022 VIA Technologies, Inc. All Rights Reserved.
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

#if !ESP32
#include <SoftwareSerial.h>
#endif

#include <Pixetto.h>

Pixetto::Pixetto(int rx, int tx)
{
  m_rx = rx;
  m_tx = tx;
}

void Pixetto::begin()
{
  const long speed = 38400;
  const int rx = m_rx;
  const int tx = m_tx;

  m_data = (struct pxt_data *) m_buf;

#if ESP32
  // https://github.com/espressif/arduino-esp32/blob/master/cores/esp32/HardwareSerial.cpp
  if ((rx == 3 && tx == 1) || (rx == 44 && tx == 43) || (rx == 20 && tx == 21)) {
    Serial.begin(speed);
    m_serial = &Serial;
    return;
  }
  if ((rx == 9 && tx == 10) || (rx == 18 && tx == 17) || (rx == 18 && tx == 19) || (rx == 15 && tx == 16)) {
    Serial1.begin(speed);
    m_serial = &Serial1;
    return;
  }
  if ((rx == 16 && tx == 17) || (rx == 19 && tx == 20)) {
    Serial2.begin(speed);
    m_serial = &Serial2;
    return;
  }
  return; // invalid pins
#else // Arduino
  // https://www.arduino.cc/reference/en/language/functions/communication/serial/
#if defined(HAVE_HWSERIAL0) && defined(HAVE_HWSERIAL1) && defined(HAVE_HWSERIAL2) && defined(HAVE_HWSERIAL3)
  // Mega, Due
  if (rx == 0 && tx == 1) {
    Serial.begin(speed);
    m_serial = &Serial;
    return;
  }
  if (rx == 19 && tx == 18) {
    Serial1.begin(speed);
    m_serial = &Serial1;
    return;
  }
  if (rx == 17 && tx == 16) {
    Serial2.begin(speed);
    m_serial = &Serial2;
    return;
  }
  if (rx == 15 && tx == 14) {
    Serial3.begin(speed);
    m_serial = &Serial3;
    return;
  }
#elif defined(HAVE_HWSERIAL1)
  // Leonardo, Micro Yun, Zero, 101
  if (rx == 0 && tx == 1) {
    Serial.begin(speed);
    m_serial = &Serial1;
    return;
  }
#elif defined(HAVE_HWSERIAL0)
  // Uno, Nano, Mini
  if (rx == 0 && tx == 1) {
    Serial.begin(speed);
    m_serial = &Serial;
    return;
  }
#endif
  SoftwareSerial *s = new SoftwareSerial(rx, tx);
  s->begin(speed);
  m_serial = s;
#endif                          // ESP32
}

void Pixetto::end()
{
}

void Pixetto::flush()
{
}

void Pixetto::enableFunc(Pixetto::EFunc fid)
{
  pxtSetFunc(*m_serial, fid);
}

long Pixetto::getVersion()
{
  return pxtGetVersion(*m_serial);
}

bool Pixetto::isDetected()
{
  static int n = 0;

  if (n == 0) {
    n = pxtAvailable(*m_serial);
    m_avail = n;
    m_seq = -1;
    // Serial.print("pxtAvailable: ");
    // Serial.println(n);
  }

  if (n > 0) {
    if (pxtGetData(*m_serial, m_buf, PXT_BUF_SIZE) > 0) {
      n--;
      m_seq++;
      // Serial.print("pxtGetData: ");
      // Serial.println(n);
      return true;
    } else {
      n = 0;
      // Serial.println("error");
    }
  }
  // Serial.println("no data");

  return false;
}

int Pixetto::getFuncID()
{
  return m_data->func_id;
}

int Pixetto::getTypeID()
{
  return m_data->class_id;
}

int Pixetto::getPosX()
{
  return m_data->x;
}

int Pixetto::getPosY()
{
  return m_data->y;
}

int Pixetto::getH()
{
  return m_data->h;
}

int Pixetto::getW()
{
  return m_data->w;
}

int Pixetto::getHeight()
{
  return m_data->h;
}

int Pixetto::getWidth()
{
  return m_data->w;
}

int Pixetto::numObjects()
{
  return m_avail;
}

int Pixetto::getSequenceID()
{
  return m_seq;
}

void Pixetto::getLanePoints(int *lx1, int *ly1, int *lx2, int *ly2,
                            int *rx1, int *ry1, int *rx2, int *ry2)
{
  *lx1 = m_data->extra.traffic.left_x1;
  *ly1 = m_data->extra.traffic.left_y1;
  *lx2 = m_data->extra.traffic.left_x2;
  *ly2 = m_data->extra.traffic.left_y2;

  *rx1 = m_data->extra.traffic.right_x1;
  *ry1 = m_data->extra.traffic.right_y1;
  *rx2 = m_data->extra.traffic.right_x2;
  *ry2 = m_data->extra.traffic.right_y2;
}

void Pixetto::getEquationExpr(char *buf, int len)
{
  memcpy(buf, m_data->extra.math.equation, min(len, 16));
}

float Pixetto::getEquationAnswer()
{
  return m_data->extra.math.result;
}

void Pixetto::getApriltagInfo(float *px, float *py, float *pz, float *rx,
                              float *ry, float *rz, float *cx, float *cy)
{
  *px = m_data->extra.apltag.pos_x;
  *py = m_data->extra.apltag.pos_y;
  *pz = m_data->extra.apltag.pos_z;

  *rx = m_data->extra.apltag.rot_x;
  *ry = m_data->extra.apltag.rot_y;
  *rz = m_data->extra.apltag.rot_z;

  *cx = m_data->extra.apltag.center_x;
  *cy = m_data->extra.apltag.center_y;
}

float Pixetto::getApriltagField(Pixetto::EApriltagField field)
{
  switch (field) {
  case Pixetto::APRILTAG_POS_X:
    return m_data->extra.apltag.pos_x;
  case Pixetto::APRILTAG_POS_Y:
    return m_data->extra.apltag.pos_y;
  case Pixetto::APRILTAG_POS_Z:
    return m_data->extra.apltag.pos_z;
  case Pixetto::APRILTAG_ROT_X:
    return m_data->extra.apltag.rot_x;
  case Pixetto::APRILTAG_ROT_Y:
    return m_data->extra.apltag.rot_x;
  case Pixetto::APRILTAG_ROT_Z:
    return m_data->extra.apltag.rot_x;
  case Pixetto::APRILTAG_CENTER_X:
    return m_data->extra.apltag.center_x;
  case Pixetto::APRILTAG_CENTER_Y:
    return m_data->extra.apltag.center_x;
  default:
    return 0;
  }

  return 0;
}

float Pixetto::getLanesField(Pixetto::ELanesField field)
{
  switch (field) {
  case Pixetto::LANES_LX1:
    return m_data->extra.traffic.left_x1;
  case Pixetto::LANES_LY1:
    return m_data->extra.traffic.left_y1;
  case Pixetto::LANES_LX2:
    return m_data->extra.traffic.left_x2;
  case Pixetto::LANES_LY2:
    return m_data->extra.traffic.left_y2;
  case Pixetto::LANES_RX1:
    return m_data->extra.traffic.right_x1;
  case Pixetto::LANES_RY1:
    return m_data->extra.traffic.right_y1;
  case Pixetto::LANES_RX2:
    return m_data->extra.traffic.right_x2;
  case Pixetto::LANES_RY2:
    return m_data->extra.traffic.right_y2;
  default:
    return 0;
  }
  return 0;
}
