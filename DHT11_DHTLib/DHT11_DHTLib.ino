#include "DHT.h"
#define DHTPIN D5     
#define DHTTYPE DHT11   // DHT 11
DHT dht(DHTPIN, DHTTYPE);

#include <Wire.h>
#include <DS3231.h>
RTClib myRtc;

#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x3F, 16, 2); //0x27 or 0x3F

void setup() {
  Serial.begin(9600);
  Serial.println("Start System");
  dht.begin();

  lcd.backlight();
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("LCD I2C");
  lcd.setCursor(3, 1);
  lcd.print("INCUBATOR");
  delay(2000);
  lcd.clear();
  lcd.init();
}
void loop() {
  delay(2000);
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.println(t);
  delay(200);
}
