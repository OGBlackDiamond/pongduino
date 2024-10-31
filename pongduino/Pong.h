#ifndef PONG_H
#define PONG_H

#include "Arduino.h"
#include "DisplayDriver.h"

class Pong {
private:
  const uint8_t paddleHeight = 3;
  const uint8_t ballSlowness = 7;
  uint8_t initialBallSlowness = 12;
  const uint8_t winThreshold = 9;

  DisplayDriver* display;

  uint8_t ballX = 3;
  uint8_t ballY = 3;

  uint8_t ballVelocityX = 1;
  uint8_t ballVelocityY = 1;

  uint8_t paddle1 = 0;
  uint8_t paddle2 = 0;

  uint8_t score1 = 0;
  uint8_t score2 = 0;

  uint8_t frameCounter = 0;

  void drawPaddle(bool, uint8_t);

  void drawPaddles();

  void drawBall();

  void drawScore(bool);

  void drawScores();

  void increaseBallSpeed();
  
  void moveBall();
  
  void reset();
  
  void beep(int frequency, int duration);

  // funy nokia ringtone
  void winTune();

  void movePaddles();

public:

  Pong (DisplayDriver* display);

  void tick();
};

#endif
