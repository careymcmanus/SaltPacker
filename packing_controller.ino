
#include <Encoder.h>
#include "EEPROMAnything.h"
#include "ButtonProcessor.h"
#include "lcdProcessor.h"

Encoder timeDial(2, 3);


const int ON_TIME_ADDRESS = 0, OFF_TIME_ADDRESS = ON_TIME_ADDRESS + sizeof(int);
const byte ON_SWITCH_PIN = 6;
const byte RELAY_PIN = 12;

const unsigned int dBnceTime =  10;
const unsigned int blinkTime = 100;
unsigned int onTime, offTime;
unsigned int eepromOnVal, eepromOffVal;

long cTime = 0;
long pRelayTime = 0;
long pBtnTime = 0;
long pBlinkTime = 0;

bool onOffState = false;
byte modeState = 0;
bool relayState = false;
long positionTimeDial = -999;

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
      updateLcdTimes(onTime, offTime);
    }
    Serial.print("Change to mode: ");
    Serial.println(modeState);
    updateLcdTimes(onTime, offTime);
    btnFlags &= ~1;
    printBinary(btnFlags);
    return btnFlags;
  }
  return btnFlags;
}

void initDial() {
  positionTimeDial = timeDial.read();
}

long checkDial(long value) {
  long newPositionTimeDial = timeDial.read();

  if (newPositionTimeDial != positionTimeDial) {
    value += 2*(newPositionTimeDial - positionTimeDial);
    serialPrintTimes();
    positionTimeDial = newPositionTimeDial;
  }
  return value;
}

void turnOn() {
  EEPROM_read(ON_TIME_ADDRESS, onTime);
  EEPROM_read(OFF_TIME_ADDRESS, offTime);
  modeState = 0;
  lcdOnScreen();
  updateLcdTimes(onTime, offTime);
}
void turnOff() {
  EEPROM_write(ON_TIME_ADDRESS, onTime);
  EEPROM_write(OFF_TIME_ADDRESS, offTime);

  lcdOffScreen();
}

void setup() {
  Serial.begin(9600);

  //Initilise LCD
  initLCD();
  initButtons();
  initDial();






  serialPrintTimes();
  Serial.print("mode state:");
  Serial.println(modeState);
  updateLcdTimes(onTime, offTime);

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
          blinkLcdOnTime(onTime);
          pBlinkTime = cTime;
        }
        onTime = checkDial(onTime);
      
        break;
      case 2:
        // ModeState 2 is the state for setting the off time for the Vibrator
        
        if (dBlinkTime >= blinkTime) {
          blinkLcdOffTime(offTime);
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
