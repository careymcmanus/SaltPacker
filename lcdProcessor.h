#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);

bool blinkState = false;

void initLCD() {
  lcd.init();
  lcd.backlight();
}

void lcdOffScreen(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("OFF");
}

void lcdOnScreen(){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("On Time:");
  lcd.setCursor(0, 1);
  lcd.print("Off Time:");
}



void updateLcdTimes(long onTime, long offTime) {
  lcd.setCursor(10, 0);
  lcd.print(onTime);
  lcd.setCursor(10, 1);
  lcd.print(offTime);
}

void blinkLcdOnTime(long onTime) {
  if (blinkState) {
    lcd.setCursor(10, 0);
    lcd.print(onTime);
  } else {
    lcd.setCursor(10, 0);
    lcd.print("     ");
  }
  blinkState = !blinkState;
}

void blinkLcdOffTime(long offTime) {
  if (blinkState) {
    lcd.setCursor(10, 1);
    lcd.print(offTime);
  } else {
    lcd.setCursor(10, 1);
    lcd.print("     ");
  }
  blinkState = !blinkState;
}
