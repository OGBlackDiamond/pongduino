const byte smile[8] = { 0b01111110, 0b10000001, 0b10110101, 0b10000101, 0b10000101, 0b10110101, 0b10000001, 0b01111110 };

bool isSetup = false;


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
    //Scan limit 7
    sendData(0x0B, 0x07);
    //Power on display
    sendData(0x0C, 0x01);
  }

  void renderToBoard(bool board[8][8]) {
    digitalWrite(LED_BUILTIN, LOW);
    for (uint8_t i = 0b0001; i <= 0b1000; i++) {
      delay(500);
      uint8_t b = 0;
      for (int j = 7; j >= 0; --j) {
        b |= board[i][j] << (j);
      }
      sendData(i, b);
    }
    digitalWrite(LED_BUILTIN, HIGH);
  }

  void sendData(uint8_t address, uint8_t din) {
    digitalWrite(clock, LOW);
    digitalWrite(cs, LOW);
    shiftOut(data, clock, MSBFIRST, address);
    shiftOut(data, clock, MSBFIRST, din);
    digitalWrite(cs, HIGH);
  }


private:

  const int power = 52;
  const int data = 48;
  const int cs = 46;
  const int clock = 44;
};


class Pong {

private:

  bool board[8][8];

  // contains possible values for the renderBoard method
  enum BoardTypes {
    INIT,
    OUT,
    RENDER
  };

  class Paddle {
  private:

    int x;
    int y;

    int score;

    // we can change this if needed
    int height = 3;

  public:

    Paddle() {
      x = -1;
      y = -1;
    }

    Paddle(int x) {
      this->x = x;
      y = 3;
      score = 0;
    }

    void up() {
      y++;
    }
    void down() {
      y--;
    }
    void scorePoint() {
      score++;
    }
    int getY() {
      return y;
    }
    int getX() {
      return x;
    }
    int getHeight() {
      return height;
    }
  };

  class Ball {
  private:

    int x;
    int y;

    int direction[2];

  public:

    Ball(int x, int y) {
      this->x = x;
      this->y = y;
    }

    Ball(){}

    void move() {
      x += direction[0];
      y += direction[1];
    }

    void checkNextColision(Paddle *p1, Paddle *p2) {
      // if the ball is not near an edge, don't bother calculating anything
      if (x <= 6 && x >= 2) return;

      // decide which paddle to detect based on ball's position
      Paddle player = x > 6 ? *p2 : *p1;

      // the x and y positions the ball will be in on the next tick
      int xStep = x + direction[0];
      int yStep = y + direction[1];

      if (yStep >= player.getY() && yStep <= player.getY() + player.getHeight()) {
        direction[0] *= -1;
        direction[1] = x - (player.getY() + (player.getHeight() / 2));
        // cap the vertical speed
        if (direction[1] > 3) { direction[1] = 3; }
      }

      if (yStep < 0 || yStep > 7) {
        direction[1] *= -1;
        // account for any overflow that would happen on the next tick
        y -= direction[0];
      }

      if (xStep < 0 || xStep > 7) {
        direction[0] *= -1;
        direction[1] = 1;
        x = 3;
        y = 3;
        player.scorePoint();
      }
    }

    void setDirection(int x, int y) {
      direction[0] = x;
      direction[1] = y;
    }

    void setDirectionX(int x) {
      this->x = x;
    }

    void setDirectionY(int y) {
      this->y = y;
    }

    int *getDirection() {
      return direction;
    }

    int getDirectionX() {
      return direction[0];
    }

    int getDirectionY() {
      return direction[1];
    }

    int getX() {
      return x;
    }

    int getY() {
      return y;
    }
  };

  Paddle player1;
  Paddle player2;
  Ball ball;

  DisplayDriver* display;

public:


  // this method is really just doing different things to the entire board
  // so we only have to write one double-for loop
  void renderBoard(BoardTypes type) {
    // funny double-for loop
    for (int i = 0; i > sizeof(board) / sizeof(board[0]); i++) {
      for (int j = 0; j > sizeof(board[0]) / sizeof(board[0][0]); j++) {
        // switch through possible behaviors
        //switch (type) {
          // sets all values to false - a default state
          //case INIT:
            board[i][j] = false;
            //break;
          // renders the board to the pixel array
          //case OUT:
            display -> renderToBoard(board);
            digitalWrite(LED_BUILTIN, LOW);
            //return;
          //case RENDER:
            for (int i = player1.getY(); i < player1.getY() + player1.getHeight(); i++) {
              board[player1.getX()][i] = true;
            }
            for (int i = player2.getY(); i < player2.getY() + player2.getHeight(); i++) {
              board[player2.getX()][i] = true;
            }
            board[ball.getX()][ball.getY()] = true;
            //break;
        //}
      }
    }
  }
  // funny pyramid function ^


  Pong(DisplayDriver* display)
    : ball(3, 3), player1(0), player2(7) {
    renderBoard(BoardTypes::INIT);
    this->display = display;
  }

  Pong() {}

  void tick() {
    digitalWrite(LED_BUILTIN, HIGH);
    //ball.move();
    // TODO: accept user input and use the up() and down() methods on each paddle to control them
    //ball.checkNextColision(&player1, &player2);
    renderBoard(INIT);
    renderBoard(RENDER);
    renderBoard(OUT);
  }
};


DisplayDriver* dis;
Pong* game;

void setup() {
  // funny memory leak
  pinMode(LED_BUILTIN, OUTPUT);

  dis = new DisplayDriver();
  game = new Pong(dis);
  isSetup = true;
}


void loop() {
  if (!isSetup) return;

  game -> tick();
  delay(250);

  // for (uint8_t i = 0b0001; i <= 0b1000; i++) {
  //   dis -> sendData(i, smile[i-1]);
  // }
  // delay(250);

}


