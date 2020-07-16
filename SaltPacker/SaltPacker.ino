
#include <Encoder.h>
#include "EEPROMAnything.h"
#include "ButtonProcessor.h"
#include "LCDProcessor.h"

Encoder timeDial(2, 3);

const byte INT_SIZE = sizeof(int);
const int ON_TIME_ADDRESS = 0, OFF_TIME_ADDRESS = ON_TIME_ADDRESS + INT_SIZE;
const byte ON_SWITCH_PIN = 6;
const byte RELAY_PIN = 12;
const byte PACKAGE_STATES_MAX = 1; // 2 states, 0 and 1;

const unsigned int dBnceTime =  10;
const unsigned int blinkTime = 100;
unsigned long onTime, offTime;


long cTime = 0;
long pRelayTime = 0;
long pBtnTime = 0;
long pBlinkTime = 0;

bool packageStateChange = false;
bool onOffState = false;
byte modeState = 0;
byte packageState = 0; // 0 is state for 230g package, 1 is state for 460g package
bool relayState = false;
long positionTimeDial = -999;

int pkgSizes[] = {230, 460};

void serialPrintTimes() {
  Serial.print("On Time: ");
  Serial.print(onTime);
  Serial.print(" Off Time: ");
  Serial.println(offTime);

}



void processRelay(long dTime) {
  if (relayState) {
    if (dTime >= onTime) {
      digitalWrite(RELAY_PIN, !relayState);
      relayState = false;
      pRelayTime = cTime;
    }
  } else {
    if (dTime >= offTime) {
      digitalWrite(RELAY_PIN, !relayState);
      relayState = true;
      pRelayTime = cTime;
    }
  }
}

uint8_t processBtnFlags(uint8_t btnFlags) {

  if (btnFlags & 1) {
    modeState++;
    if (modeState > 2) {
      modeState = 0;
    }
    updateLcdTimes(onTime, offTime);
    btnFlags &= ~1;
    return btnFlags;
  }
  if (btnFlags & (1 << 1)) {
    writeEepromTimes();
    packageState++;
    if (packageState > PACKAGE_STATES_MAX) {
      packageState = 0;
    }
    packageStateChange = true;
    btnFlags &= ~(1 << 1);
  }
  return btnFlags;
}

void initDial() {
  positionTimeDial = timeDial.read();
}

long checkDial(long value) {
  long newPositionTimeDial = timeDial.read();

  if (newPositionTimeDial != positionTimeDial) {
    value += 2 * (newPositionTimeDial - positionTimeDial);
    serialPrintTimes();
    positionTimeDial = newPositionTimeDial;
  }
  return value;
}

void getEepromTimes() {
  EEPROM_read(ON_TIME_ADDRESS + 2 * packageState * INT_SIZE, onTime);
  EEPROM_read(OFF_TIME_ADDRESS + 2 * packageState * INT_SIZE, offTime);
}

void writeEepromTimes() {
  EEPROM_write(ON_TIME_ADDRESS + 2 * packageState * INT_SIZE, onTime);
  EEPROM_write(OFF_TIME_ADDRESS + 2 * packageState * INT_SIZE, offTime);
}

void turnOn() {
  getEepromTimes();
  modeState = 0;
  lcdOnScreen();
  updatePkgSize(pkgSizes[packageState]);
  updateLcdTimes(onTime, offTime);
}
void turnOff() {
  writeEepromTimes();
  lcdOffScreen();
}

void setup() {
  Serial.begin(9600);

  //Initilise LCD
  initLCD();
  //Initialise Buttons
  initButtons();
  //Initialise Dial
  initDial();

  pinMode(RELAY_PIN, OUTPUT);
  pinMode(ON_SWITCH_PIN, INPUT_PULLUP);
}

void loop() {
  static bool firstOff = true;
  static bool firstOn = true;
  
  onOffState = digitalRead(ON_SWITCH_PIN);

  if (onOffState) {
    if (firstOn) {
      turnOn();
      firstOn = false;
      firstOff = true;
    }

    if (packageStateChange) {
      getEepromTimes();
      updateLcdTimes(onTime, offTime);
      updatePkgSize(pkgSizes[packageState]);
      packageStateChange = false;
    }

    // Vibrator runs no matter what mode state its in.
    cTime = millis();
    long dBtnTime = cTime - pBtnTime;
    long dRelayTime = cTime - pRelayTime;
    long dBlinkTime = cTime - pBlinkTime;

    processRelay(dRelayTime);


    switch (modeState) {
      case 0:
        // ModeState 0 is the default running mode just looks for mode button to be pressed
        break;
      case 1:
        // ModeState 1 is the state for setting the onTime for the Vibrator
        if (dBlinkTime >= blinkTime) {
          blinkLcdTime(onTime, 0);
          pBlinkTime = cTime;
        }
        onTime = checkDial(onTime);

        break;
      case 2:
        // ModeState 2 is the state for setting the off time for the Vibrator

        if (dBlinkTime >= blinkTime) {
          blinkLcdTime(offTime, 1);
          pBlinkTime = cTime;
        }
        offTime = checkDial(offTime);
        break;
    }

    if (dBtnTime >= dBnceTime) {
      buttonFlags = getButtonFlags();

    }
    buttonFlags = processBtnFlags(buttonFlags);

  } else { //Else if in off state check for on button pressed
    firstOn = true;
    if (firstOff) {
      turnOff();
      firstOff = false;
    }
  }
}
