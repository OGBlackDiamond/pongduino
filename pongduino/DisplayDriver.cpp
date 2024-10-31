#include "DisplayDriver.h"

const uint8_t digits[10][3] = { { 0b00011111, 0b00010001, 0b00011111 }, { 0b00000000, 0b00000000, 0b00011111 }, { 0b00011101, 0b00010101, 0b00010111 }, { 0b00010101, 0b00010101, 0b00011111 }, { 0b00000111, 0b00000100, 0b00011111 }, { 0b00010111, 0b00010101, 0b00011101 }, { 0b00011111, 0b00010101, 0b00011101 }, { 0b00000001, 0b00000001, 0b00011111 }, { 0b00011111, 0b00010101, 0b00011111 }, { 0b00010111, 0b00010101, 0b00011111 } };

DisplayDriver::DisplayDriver(uint8_t resolutionX, uint8_t resolutionY) {
  if (resolutionX != resolutionY || resolutionX % 0x08 != 0x00 || resolutionY % 0x08 != 0x00) {Serial.println("invalid arguments");}
  resX = resolutionX;
  resY = resolutionY;

  buffer = new bool*[resX];

  for (int i = 0; i < resolutionX; i++) {
    buffer[i] = new bool[resY];
  }


  //Set all pins to output mode
  pinMode(data, OUTPUT);
  pinMode(cs, OUTPUT);
  pinMode(clock, OUTPUT);
  
  digitalWrite(cs, LOW);

  //Shut down
  sendData(0x0C, 0x00);
  //Decode mode
  sendData(0x09, 0x00);
  //Set brightness to 1/16
  sendData(0x0A, 0x00);
  //Disable test mode
  sendData(0x0F, 0x00);
  //Scan limit 7
  sendData(0x0B, 0x07);
  //Power on display
  sendData(0x0C, 0x01);

  //Clear screen
  clearScreen();
}

void DisplayDriver::sendData(uint8_t address, uint8_t din) {
    digitalWrite(clock, LOW);
    digitalWrite(cs, LOW);
    shiftOut(data, clock, MSBFIRST, address);
    shiftOut(data, clock, MSBFIRST, din);
    digitalWrite(cs, HIGH);
}

// sends data, but doesn't interact with the CS pin
// this should be used for daisy-chaining
void DisplayDriver::sendDataLow(uint8_t address, uint8_t din) {
  shiftOut(data, clock, MSBFIRST, address);
  shiftOut(data, clock, MSBFIRST, din);
}

// clears all displays
void DisplayDriver::clearScreen()
{
  clearBuffer();
  renderDisplay();
}

void DisplayDriver::clearBuffer()
{
  for (int i = 0; i < resX; i++) 
  {
    for (int j = 0; j < resY; j++) {
      buffer[i][j] = false;
    }
  }
}

void DisplayDriver::setPixel(int x, int y, bool state)
{
  if (x >= resX || x < 0 || y >= resY || y < 0) return;
  buffer[x][y] = state;
}

void DisplayDriver::testDisplay()
{
  sendData(0x0F, 0x01);
}

// we might not need this anymore
void DisplayDriver::drawColumn(uint8_t column, uint8_t value)
{
  //Don't draw the column if it isn't a display register
  if (column < 0b0000 || column > 0b0111) return;
  //MAX7219 display registers start at 0x01 instead of 0
  sendData(column + 1, value);
}

// converts the first 8 boolean values in an array to a byte
uint8_t DisplayDriver::boolsToByte(const bool *bits) {
  uint8_t byte = 0;
    for (int i = 0; i < 8; i++) {
      if (bits[i]) {
        byte |= 1 << (7 - i);
      }
    }
  return byte;
}

// renders whatever content is currently in the buffer
void DisplayDriver::renderDisplay() {

  // loops through half of the columns in the buffer
  for (int i = 0; i < resY / 2; i++) {
    bool bytes[(resX + resY) / 8][8];
    int segSwitch = 0;
    // loops through all rows
    for (int j = 0; j < resX; j++) {
      if (j >= resX / 2) {segSwitch = 1;}
      bytes[0 + segSwitch][7-(j - (8 * segSwitch))] = buffer[j][i];
      bytes[2 + segSwitch][7-(j - (8 * segSwitch))] = buffer[j][i+8];
    }

    digitalWrite(cs, LOW);
    for (bool* byte : bytes) {
      sendDataLow(i + 1, boolsToByte(byte));
    }
    digitalWrite(cs, HIGH);

    // memory safety :)

    //Kinda cursed but I wanted an easy way to turn this on and off
    if (false)
    {
      for (int i = 0; i < resX; i++) {
        for (int j = 0; j < resY; j++) {
          if (buffer[i][j]) {
            Serial.write("O");
          } else {
            Serial.write(" ");
          }
        }
        Serial.write("\n");
      }
      Serial.println();
    }
  }  
}

int sign(int value)
{
  if (value > 0) return 1;
  if (value == 0) return 0;
  if (value < 0) return -1;
}

void DisplayDriver::drawLine(int x1, int y1, int x2, int y2)
{
  int y = y1;
  int x = x1;
  int dx = abs(x2-x1);
  int dy = abs(y2-y1);
  int s1 = sign(x2-x1);
  int s2 = sign(y2-y1);

  bool interchange;

  if (dy > dx)
  {
    int t = dx;
    dx = dy;
    dy = t;
    interchange = true;
  } 
  else {interchange = false;}
  
  int e = 2*dy - dx;
  int a = 2*dy;
  int b = 2*dy - 2*dx;
  setPixel(x, y, true);

  for (int i = 1; i <= dx; i++)
  {
    if (e < 0) 
    {
      if (interchange) {y += s2;}
      else {x += s1;}
      e += a;
    }
    else
    {
      y += s2;
      x += s1;
      e += b;
    }
    setPixel(x, y, true);
  }
}

void DisplayDriver::drawLine(VectorI p1, VectorI p2)
{
  drawLine(p1[0], p1[1], p2[0], p2[1]);
}

void DisplayDriver::drawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, uint16_t color)
{
  VectorI p1 = VectorI(x1, y1);
  VectorI p2 = VectorI(x2, y2);
  VectorI p3 = VectorI(x3, y3);
  drawLine(p1, p2);
  drawLine(p2, p3);
  drawLine(p3, p1);
}
