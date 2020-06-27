struct Btn {
  byte pin;
  uint8_t b_acc; // the button accumulator
  uint8_t b_val; // the boolean button val
};

const uint8_t NUM_B = 2;


unsigned int currentTime = 0, previousTime = 0;
uint8_t buttonFlags = 0;

Btn modeBtn = {5, 0, 0};
Btn nullBtn = {7, 0, 0};

Btn btnData[NUM_B] = {modeBtn, nullBtn};

void printBinary(uint8_t number) {
  for (int i = 0; i < 8; i++) {
    Serial.print(bitRead(number, i));
  }
  Serial.println();
}

void initButtons() {
  for (int i = 0; i < NUM_B; i++) {
    pinMode(btnData[i].pin, INPUT_PULLUP);
  }
}
uint8_t filterButtonN8(Btn *button) {
  button->b_acc <<= 1; // shift out oldest state, make new state a zero
  bool pinState = digitalRead(button->pin);
  if (!pinState) {
    button->b_acc |= 1;
    if (button->b_acc == 255) {
      button->b_val = 1;

    } else {
      button->b_val = 0;
    }
  }
  return button->b_val;
}

uint8_t filterAllButtons() {
  int i;
  uint8_t b;
  uint8_t buttons = 0x00;
  for (i = NUM_B - 1; i >= 0; i--)
  {
    b = filterButtonN8(&btnData[i]);
    buttons = (buttons << 1) | b;
  }
  return buttons;
}

uint8_t getButtonFlags() {
  uint8_t btns;
  uint8_t newButtons;
  static uint8_t lastButtons = 0;

  btns = filterAllButtons();
  newButtons = btns & ~lastButtons;
  buttonFlags |= newButtons;
  lastButtons = btns;
  return buttonFlags;

}
