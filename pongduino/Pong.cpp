#include "Pong.h"

void Pong::drawPaddle(bool side, uint8_t height) {
    //2^n in binary gives a single bit on shifted n positions over. 
    //By subtracting 1 from this, it turns on all bits before the 1, created a paddle of size n when rendered.
    uint8_t paddle = (uint8_t)(pow(2, paddleHeight));
    //Shift paddle value up or down based on paddle position before rendering.
    paddle = paddle << (side ? paddle1 : paddle2);
    display->drawColumn(side ? 7 : 0, paddle);
}

void Pong::drawPaddles() {
    drawPaddle(false, paddleHeight);
    drawPaddle(true, paddleHeight);
}

void Pong::drawBall() {
    display->drawPixel(ballX, ballY);
}

void Pong::drawScore(bool side) {
    uint8_t offset = side ? 0 : 5;
    for (int i = 2; i >= 0; i--)
    {
      display->drawColumn(i + offset, digits[(side ? score2 : score1)][i]);
    }
}

void Pong::drawScores() {
    drawScore(true);
    drawScore(false);
}

void Pong::increaseBallSpeed() {
    if (initialBallSlowness > ballSlowness) {
      initialBallSlowness--;
    }
}
  
void Pong::moveBall() {

    //Move the ball by its velocity
    ballX += ballVelocityX;
    ballY += ballVelocityY;

    //Bounce ball off top and bottom of the screen
    ballVelocityY = ballY > 6 ? -1 : ballY < 1 ? 1 : ballVelocityY;

    //Check if paddles are blocking the ball
    //Bounce the ball and play a noise if it is, add a score point and reset otherwise
    if (ballX > 5 && paddle1 <= ballY && paddle1 + paddleHeight > ballY)
    {
      ballVelocityX *= -1;
      if (ballY == paddle1) {ballVelocityY = -1;}
      else if (ballY == paddle1 + 1 && paddle1 != 0 && paddle1 + paddleHeight != 8) {ballVelocityY = 0;}
      else if (ballY == paddle1 + paddleHeight-1) {ballVelocityY = 1;}
      increaseBallSpeed();
      beep(550, 20);
    } else if (ballX > 7 && ballX != 255) {
      score2++;
      reset();
      return;
    }
    if (ballX < 2 && paddle2 <= ballY && paddle2 + paddleHeight > ballY)
    {
      ballVelocityX *= -1;
      if (ballY == paddle2) {ballVelocityY = -1;}
      else if (ballY == paddle2 + 1 && paddle2 != 0 && paddle2 + paddleHeight != 8) {ballVelocityY = 0;}
      else if (ballY == paddle2 + paddleHeight-1) {ballVelocityY = 1;}
      increaseBallSpeed();
      beep(450, 20);
    } else if (ballX == 255) {
      score1++;
      reset();
      return;
    }
}

void Pong::reset()
{
    beep(450, 100);
    beep(550, 100);
    beep(650, 100);

    //Reset values
    ballX = random(3, 5);
    ballY = random(3, 5);
    ballVelocityX = (random(2) == 1) ? -1 : 1;
    ballVelocityY = (random(2) == 1) ? -1 : 1;
    frameCounter = 0;
    initialBallSlowness = 12;

    display->clearScreen();
    drawScores();
    delay(1000);

    if (score1 >= winThreshold || score2 >= winThreshold)
    {
      winTune();
      score1 = 0;
      score2 = 0;
      reset();
      return;
    }

    //Draw first frame to freeze on for a second
    display->clearScreen();
    movePaddles();
    drawPaddles();
    drawBall();
    delay(1000);
}

void beep(int frequency, int duration) {
    tone(A14, frequency);
    delay(duration);
    noTone(A14);
}

// funy nokia ringtone
void Pong::winTune() {
    beep(659.25, 166);
    beep(587.33, 166);
    beep(369.99, 333);
    beep(415.3, 333);
    beep(554.37, 166);
    beep(493.88, 166);
    beep(293.66, 333);
    beep(329.63, 333);
    beep(493.88, 166);
    beep(440, 166);
    beep(277.18, 333);
    beep(329.63, 333);
    beep(440, 666);
    delay(2000);
}

void Pong::movePaddles() {
    //Move paddles based on potentiometer values in the analog ports
    paddle2 = (1023 - analogRead(A0)) / (1023 / (8 - paddleHeight));
    paddle1 = (1023 - analogRead(A15)) / (1023 / (8 - paddleHeight));
}

Pong::Pong (DisplayDriver* display) {
    this->display = display;
    reset();
}

void Pong::tick() {
    display->clearScreen();
    movePaddles();
    drawPaddles();
    drawBall();

    frameCounter++;
    if (frameCounter < initialBallSlowness) return;

    moveBall();

    frameCounter = 0;
}


