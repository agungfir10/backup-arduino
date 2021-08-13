/* ---------- LIBRARIES ----------- */
#include <LiquidCrystal_I2C.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Servo.h>

// define sensor on pin 6
#define ONE_WIRE_BUS_1 2
#define ONE_WIRE_BUS_2 3

// setup sensor
OneWire oneWire1(ONE_WIRE_BUS_1);
OneWire oneWire2(ONE_WIRE_BUS_2);

// berikan nama variabel,masukkan ke pustaka Dallas
DallasTemperature libTempMieCup(&oneWire1);
DallasTemperature libTempMieInstant(&oneWire2);

// lcd
LiquidCrystal_I2C lcd(0x27, 16, 2);

//iniatate temperature now
float tempNowMieCup = 00.00;
float tempNowMieInstant = 00.00;

// RELAY
int relayMieInstant = 6;
int relayMieCup = 7;

int statusRelayMieCup = 0;
int statusRelayMieInstant = 0;

// defines pins numbers
const int trigPin = 5;
const int echoPin = 4;

// defines variables
long duration;
int distance;

// servo
const int servo = 8;
Servo myServo;

int statusServo=0;

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(relayMieCup, OUTPUT);
  pinMode(relayMieInstant, OUTPUT);

  // default relay off
  digitalWrite(relayMieCup, LOW);
  digitalWrite(relayMieInstant, LOW);

  // lcd i2c 16x2 setup
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("WELCOME 2 SYSTEM");
  lcd.setCursor(0, 1);
  lcd.print("  START SYSTEM");
  lcd.clear();
  // ultrasonic
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT);  // Sets the echoPin as an Input

  // temperature setup
  libTempMieCup.begin();
  libTempMieInstant.begin();

  // servo
  myServo.attach(9);
  myServo.write(0);
  
}

void loop()
{
  //  put your main code here, to run repeatedly:
  if (Serial.available() > 0)
  {
    char receive=Serial.read();
//    Serial.println("DATA FROM ANDROID : "+String(receive));
    /*
     * A = statusServo ON
     * B = statusServo OFF
     * C = statusRelayMieInstant ON
     * D = statusRelayMieInsant OFF
     * E = statusRelayMieCup ON
     * F = statusRelayMieCup OFF
     * G =
     * H =
     */
    if(receive == "A"){
      statusServo=1;
    }else if(receive == "B"){
      statusServo=0;
    }else if(receive == "C"){
      statusRelayMieInstant=1;
    }else if(receive == "D"){
      statusRelayMieInstant=0;
    }else if(receive == "E"){
      statusRelayMieCup=1;
    }else if(receive == "F"){
      statusRelayMieCup=0;
    }else if(receive == "G"){
      
    }else if(receive == "H"){
      
    }else{
      
    }
  }

  if(statusServo==1){
    myServo.write(90);
  }else{
    myServo.write(0);
  }
  relayMieInstantFunction();
  relayMieCupFunction();
  showTempInLCD();

  getTemperature();
  getDistance();
//  Serial.println("TEMP MIE CUP: "+String(tempNowMieCup));
//  Serial.println("TEMP MIE INSTANT: "+String(tempNowMieInstant));
//  Serial.println("DISTANCE : "+String(distance));
  Serial.println("#" + String(round(tempNowMieCup)) + "+" + String(round(tempNowMieInstant)) + String(distance) + "+" + String(statusRelayMieInstant) + "+" + String(statusRelayMieInstant) + "~");
//  Serial.println("#" + String(random(20,30)) + "+" + String(random(20,40)) + "+" + String(random(20,30)) + "+" + statusRelayMieInstant + "+" + statusRelayMieInstant + "~");
  delay(500);
}

void getTemperature()
{
  libTempMieCup.requestTemperatures();
  tempNowMieCup = libTempMieCup.getTempCByIndex(0);
}

void relayMieInstantFunction()
{
  if(statusRelayMieInstant==1){
    digitalWrite(relayMieInstant, HIGH);
  }else{
    digitalWrite(relayMieInstant, LOW);
  }
}

void relayMieCupFunction()
{
  if(statusRelayMieCup==1){
    digitalWrite(relayMieCup, HIGH);
  }else{
    digitalWrite(relayMieCup, LOW);
  }
}

void getDistance()
{
  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  // Calculating the distance
  distance = duration * 0.034 / 2;
}

void showTempInLCD()
{
  lcd.setCursor(0, 0);
  lcd.print("TEMP M INS :" + String(round(tempNowMieInstant)));
  lcd.setCursor(0, 1);
  lcd.print("TEMP M CUP :" + String(round(tempNowMieCup)));
}
