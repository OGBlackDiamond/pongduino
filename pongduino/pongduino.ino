const byte smile[8] = { 0b00111100, 0b01000010, 0b10110101, 0b10000101, 0b10000101, 0b10110101, 0b01000010, 0b00111100 };

bool isSetup = false;

DisplayDriver display;
Pong game;

void setup() {
  // funny memory leak
  display = new DisplayDriver();
  game = new Pong(display);
  isSetup = true;
}

void loop() {
  if (!isSetup) return;

  for (uint8_t i = 0b0001; i <= 0b1000; i++) {
    display.sendData(i, smile[i - 1]);
  }
  //delay(500);
}


