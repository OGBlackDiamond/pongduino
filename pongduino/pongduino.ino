#include "DisplayDriver.h"
#include "Pong.h"

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

const uint8_t framerate = 60;

DisplayDriver* dis;
Pong* pong;

bool isSetup = false;

void setup() {
  //Establish serial connection
  Serial.begin(9600);

  dis = new DisplayDriver();
  pong = new Pong(dis);
  isSetup = true;
}


void loop() {
  if (!isSetup) return;
  pong -> Tick();
  delay(1000 / framerate);
}


