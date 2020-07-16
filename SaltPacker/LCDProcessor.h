#include <LiquidCrystal_I2C.h>

const int TIME_CURSOR = 5;

LiquidCrystal_I2C lcd(0x27, 20, 4);

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
  lcd.print("On:");
  lcd.setCursor(0, 1);
  lcd.print("Off:");
}

void updatePkgSize(int pkgSize){
  lcd.setCursor(12, 0);
  lcd.print(pkgSize);
  lcd.setCursor(15, 0);
  lcd.print("g");
}


void updateLcdTimes(long onTime, long offTime) {
  //Clear the previous onTime value 
  lcd.setCursor(TIME_CURSOR, 0);
  lcd.print("       ");
  //Print the new value
  lcd.setCursor(TIME_CURSOR, 0);
  lcd.print(onTime);
  //Clear the previous offTime value
  lcd.setCursor(TIME_CURSOR, 1);
  lcd.print("       ");
  //Print the new value
  lcd.setCursor(TIME_CURSOR, 1);
  lcd.print(offTime);
 
}

void blinkLcdTime(long bTime, int rowPos) {
  static bool blinkState = false;
  if (blinkState) {
    lcd.setCursor(TIME_CURSOR, rowPos);
    lcd.print(bTime);
  } else {
    lcd.setCursor(TIME_CURSOR, rowPos);
    lcd.print("     ");
  }
  blinkState = !blinkState;
}
