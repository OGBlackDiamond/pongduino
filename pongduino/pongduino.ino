const int power = 52;
//const int ground = 50;
const int data = 48;
const int cs = 46;
const int clock = 44;

const byte smile[8] = {0b00111100, 0b01000010, 0b10110101, 0b10000101, 0b10000101, 0b10110101, 0b01000010, 0b00111100};

bool isSetup = false;

void sendData(uint8_t address, uint8_t din) {
  digitalWrite(cs, LOW);
  shiftOut(data, clock, MSBFIRST, address);
  shiftOut(data, clock, MSBFIRST, din);
  digitalWrite(cs, HIGH);
}

void setup()
{
  //pinMode(LED_BUILTIN, OUTPUT);

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

  isSetup = true;
}

void loop()
{
  if (!isSetup) return;

  for (uint8_t i = 0b0001; i <= 0b1000; i++)
  {
    sendData(i, smile[i-1]);
  }
  //delay(500);
}

