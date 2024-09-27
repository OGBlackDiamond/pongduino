const uint8_t digits[10][3] = { { 0b00011111, 0b00010001, 0b00011111 }, { 0b00000000, 0b00000000, 0b00011111 }, { 0b00011101, 0b00010101, 0b00010111 }, { 0b00010101, 0b00010101, 0b00011111 }, { 0b00000111, 0b00000100, 0b00011111 }, { 0b00010111, 0b00010101, 0b00011101 }, { 0b00011111, 0b00010101, 0b00011101 }, { 0b00000001, 0b00000001, 0b00011111 }, { 0b00011111, 0b00010101, 0b00011111 }, { 0b00010111, 0b00010101, 0b00011111 } };

class DisplayDriver {
public:

  DisplayDriver() {
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

  void sendData(uint8_t address, uint8_t din) {
    digitalWrite(clock, LOW);
    digitalWrite(cs, LOW);
    shiftOut(data, clock, MSBFIRST, address);
    shiftOut(data, clock, MSBFIRST, din);
    digitalWrite(cs, HIGH);
  }

  void drawColumn(uint8_t column, uint8_t value)
  {
    //Don't draw the column if it isn't a display register
    if (column < 0b0000 || column > 0b0111) return;
    //MAX7219 display registers start at 0x01 instead of 0
    sendData(column + 1, value);
  }

  //WARNING - This function will actually draw a full column and should only be used in cases where there is only 1 pixel in the column.
  void drawPixel(uint8_t x, uint8_t y)
  {
    drawColumn(x, 0b00000001 << y);
  }

  void clearScreen()
  {
    //Sens 0 to every display register
    for (uint8_t i = 0; i < 8; i++) {
      drawColumn(i, 0);
    }
  }

private:

  const int data = 48;
  const int cs = 46;
  const int clock = 44;
};

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

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

  void drawPaddle(bool side, uint8_t height)
  {
    //2^n in binary gives a single bit on shifted n positions over. 
    //By subtracting 1 from this, it turns on all bits before the 1, created a paddle of size n when rendered.
    uint8_t paddle = (uint8_t)(pow(2, paddleHeight));
    //Shift paddle value up or down based on paddle position before rendering.
    paddle = paddle << (side ? paddle1 : paddle2);
    display->drawColumn(side ? 7 : 0, paddle);
  }

  void drawPaddles()
  {
    drawPaddle(false, paddleHeight);
    drawPaddle(true, paddleHeight);
  }

  void drawBall()
  {
    display->drawPixel(ballX, ballY);
  }

  void drawScore(bool side)
  {
    uint8_t offset = side ? 0 : 5;
    for (int i = 2; i >= 0; i--)
    {
      display->drawColumn(i + offset, digits[(side ? score2 : score1)][i]);
    }
  }

  void drawScores()
  {
    drawScore(true);
    drawScore(false);
  }

  void increaseBallSpeed() {
    if (initialBallSlowness > ballSlowness) {
      initialBallSlowness--;
    }
  }
  
  void moveBall()
  {

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

  void reset()
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

  void beep(int frequency, int duration)
  {
    tone(A14, frequency);
    delay(duration);
    noTone(A14);
  }

  // funy nokia ringtone
  void winTune()
  {
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

  void movePaddles()
  {
    //Move paddles based on potentiometer values in the analog ports
    paddle2 = (1023 - analogRead(A0)) / (1023 / (8 - paddleHeight));
    paddle1 = (1023 - analogRead(A15)) / (1023 / (8 - paddleHeight));
  }

public:

  Pong (DisplayDriver* display)
  {
    this->display = display;
    reset();
  }

  void Tick()
  {
    display->clearScreen();
    movePaddles();
    drawPaddles();
    drawBall();

    frameCounter++;
    if (frameCounter < initialBallSlowness) return;

    moveBall();

    frameCounter = 0;
  }
};

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


