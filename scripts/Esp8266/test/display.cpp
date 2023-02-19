#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ESP8266WiFi.h>

#define COLUMS           20   //LCD columns
#define ROWS             4    //LCD rows
#define LCD_SPACE_SYMBOL 0x20 //space symbol from LCD ROM, see p.9 of GDM2004D datasheet

LiquidCrystal_I2C lcd(PCF8574_ADDR_A21_A11_A01, 4, 5, 6, 16, 11, 12, 13, 14, POSITIVE);

void setup()
{
  WiFi.persistent(false);           //disable saving wifi config into SDK flash area
  WiFi.forceSleepBegin();           //disable AP & station by calling "WiFi.mode(WIFI_OFF)" & put modem to sleep

  Serial.begin(115200);

  while (lcd.begin(COLUMS, ROWS, LCD_5x8DOTS, 4, 5, 400000, 250) != 1) //colums, rows, characters size, SDA, SCL, I2C speed in Hz, I2C stretch time in usec 
  {
    Serial.println(F("PCF8574 is not connected or lcd pins declaration is wrong. Only pins numbers: 4,5,6,16,11,12,13,14 are legal."));
    delay(5000);
  }

  lcd.print(F("PCF8574 is OK...")); //(F()) saves string to flash & keeps dynamic memory free
  delay(2000);

  lcd.clear();

  /* prints static text */
  lcd.setCursor(0, 1);              //set 1-st colum & 2-nd row, 1-st colum & row started at zero
  lcd.print(F("Hello world!"));
  lcd.setCursor(0, 2);              //set 1-st colum & 3-rd row, 1-st colum & row started at zero
  lcd.print(F("Random number:"));
}

void loop()
{
  /* prints dynamic text */
  lcd.setCursor(14, 2);             //set 15-th colum & 3-rd  row, 1-st colum & row started at zero
  lcd.print(random(10, 1000));
  lcd.write(LCD_SPACE_SYMBOL);

  delay(1000);
}