const byte smile[8] = {0b00111100, 0b01000010, 0b10110101, 0b10000101, 0b10000101, 0b10110101, 0b01000010, 0b00111100};

bool isSetup = false;

class Pong;

DisplayDriver display;
Pong game;

void setup()
{
  // funny memory leak
  display = new DisplayDriver();
  game = new Pong(display);
  isSetup = true;
}

void loop()
{
  if (!isSetup) return;

  for (uint8_t i = 0b0001; i <= 0b1000; i++)
  {
    display.sendData(i, smile[i-1]);
  }
  //delay(500);
}

class DisplayDriver {
public:

    DisplayDriver() {
        pinMode(power, OUTPUT);
        pinMode(data, OUTPUT);
        pinMode(cs, OUTPUT);
        pinMode(clock, OUTPUT);

        digitalWrite(power, HIGH);
        digitalWrite(cs, LOW);

        //Decode mode
        sendData(0x09, 0x00);
        delay(500);
        //Set brightness to 1/16
        sendData(0x0A, 0x00);
        delay(500);
        //Scan limit 0
        sendData(0x0B, 0x07);
        delay(500);
        //Power on display
        sendData(0x0C, 0x01);
        delay(500);
    }

    void renderToBoard(boolean **board) {
        for (int i = 0x00; i < 0x10; ++i) {
            for (int j = 0x00; j < 0x10; ++j) {
                if (board[i][j]) {
                    sendData(i, j);
                }
            }
        }
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

