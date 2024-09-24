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
    for (uint8_t i = 0b0001; i <= 0b1000; i++) {
      sendData(i, 0);
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
  const uint8_t paddleHeight = 2;
  const uint8_t ballSlowness = 5;


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

  void drawBall()
  {
    display->drawPixel(ballX, ballY);
  }

  void moveBall()
  {
    //Move the ball by its velocity
    ballX += ballVelocityX;
    ballY += ballVelocityY;

    //Bounce ball off top and bottom of the screen
    if (ballY > 6 || ballY < 1) ballVelocityY *= -1;

    //Check if paddles are blocking the ball
    //Bounce the ball and play a noise if it is, add a score point and reset otherwise
    if (ballX > 5 && paddle1 <= ballY && paddle1 + paddleHeight > ballY)
    {
      ballVelocityX *= -1;
      beep(450, 20);
    } else {
      score2++;
      reset();
    }
    if (ballX < 2 && paddle2 <= ballY && paddle2 + paddleHeight > ballY)
    {
      ballVelocityX *= -1;
      beep(450, 20);
    } else {
      score1++;
      reset();
    }
  }

  void reset()
  {
    beep(350, 700);

    //Reset values
    ballX = 3;
    ballY = 3;
    ballVelocityX = 1;
    ballVelocityY = 1;
    frameCounter = 0;

    //Draw first frame to freeze on for a second
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

  void movePaddles()
  {
    //Move paddles based on potentiometer values in the analog ports
    paddle2 = (1023 - analogRead(A0)) / (1023 / (8 - paddleHeight));
    paddle1 = (1023 - analogRead(A15)) / (1023 / (8 - paddleHeight));
  }

  void drawPaddles()
  {
    drawPaddle(false, paddleHeight);
    drawPaddle(true, paddleHeight);
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

    frameCounter++;
    if (frameCounter < ballSlowness) return;

    moveBall();
    drawBall();

    frameCounter = 0;
  }
};

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

const uint8_t framerate = 20;

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


