class DisplayDriver {
public:

  DisplayDriver() {
    pinMode(power, OUTPUT);
    pinMode(data, OUTPUT);
    pinMode(cs, OUTPUT);
    pinMode(clock, OUTPUT);
    
    digitalWrite(power, HIGH);
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
    sendData(column + 1, value);
  }

  void clearScreen()
  {
    for (uint8_t i = 0b0001; i <= 0b1000; i++) {
      sendData(i, 0);
    }
  }

private:

  const int power = 52;
  const int data = 48;
  const int cs = 46;
  const int clock = 44;
};

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

class Pong {

private:
  const uint8_t paddleHeight = 4;
  const uint8_t ballSlowness = 5;

  DisplayDriver* display;

  uint8_t ballX = 3;
  uint8_t ballY = 3;

  uint8_t ballVelocityX = 1;
  uint8_t ballVelocityY = 1;

  uint8_t paddle1 = 0;
  uint8_t paddle2 = 0;

  bool bounce = false;

  uint8_t frameCounter = 0;

  void drawPaddle(bool side, uint8_t height)
  {
    display->sendData(side ? 8 : 1, (uint8_t)(pow(2, paddleHeight)) << (side ? paddle1 : paddle2));
  }

  void drawBall()
  {
    display->sendData(ballX + 1, 0b00000001 << ballY);
  }

  void moveBall()
  {
    ballX += ballVelocityX;
    ballY += ballVelocityY;

    if (ballY > 6 || ballY < 1) ballVelocityY *= -1;

    if (ballX > 5 && paddle1 <= ballY && paddle1 + paddleHeight > ballY)
    {
      ballVelocityX *= -1;
      beep(450, 20);
    }
    if (ballX < 2 && paddle2 <= ballY && paddle2 + paddleHeight > ballY)
    {
      ballVelocityX *= -1;
      beep(450, 20);
    }
  }

  void beep(int frequency, int duration)
  {
    tone(A14, frequency);
    delay(duration);
    noTone(A14);
  }

  void movePaddles()
  {
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
  pinMode(LED_BUILTIN, OUTPUT);
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


