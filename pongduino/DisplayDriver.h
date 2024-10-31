#ifndef DISPLAYDRIVER_H
#define DISPLAYDRIVER_H

#include "Arduino.h"


/*

The 16x16 dot matrix is split into 4 8x8 matricies that are daisy chained together

This is their order

[4][2]
[3][1]

*/

class DisplayDriver {
public:
  uint8_t resX;
  uint8_t resY;
    
  DisplayDriver(uint8_t, uint8_t);

  void clearScreen();

  void clearBuffer();

  void setPixel(int, int, bool);

  void drawLine(int, int, int, int);

  void drawTriangle(int, int, int, int, int, int, uint16_t);

  void testDisplay();

  void renderDisplay();

  virtual void drawTriangle(int, int, int, int, int, int, uint16_t);

  virtual void testDisplay();

  virtual void renderDisplay();

private:

  bool** buffer;

  const uint8_t clock = 44;
  const uint8_t cs = 46;
  const uint8_t data = 48;

  void sendData(uint8_t, uint8_t);

  void sendDataLow(uint8_t, uint8_t);

  void drawColumn(uint8_t, uint8_t);

  uint8_t boolsToByte(const bool* bits);
};

#endif
