/// bot
#include <CTBot.h>
// lcd
#include <LiquidCrystal_I2C.h>
// library wifi
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

#include <SPI.h>
#include <MFRC522.h> // import file MFRC522 yang sudah kita install
#include <Servo.h>

#define TOKEN "1830803564:AAErMCHe4wzTtKO3hLraP63WnO95kR_LPQQ"
CTBot myBot;

LiquidCrystal_I2C lcd(0x27, 16, 2); //0x27 or 0x3F

String code_reg;

#define SDA_PIN D4                 // mendefinisikan SDA pin ke D2
#define RST_PIN D3                 // mendefinisikan RST pin ke D1
MFRC522 mfrc522(SDA_PIN, RST_PIN); // setting mfrc dengan SDA dan RST

// lampu / relay
#define RELAY D8
#define RELAY_OFF 1
#define RELAY_ON 0

// web server
#define BASE_URL "http://smart-gallon.000webhostapp.com"

// wifi
#define WIFI_SSID "Wakil President Jancukers"
#define WIFI_PASS "jancukers"
ESP8266WiFiMulti WiFiMulti;

String urlSaveHistory = String(BASE_URL) + "/save_history?tag=";
String urlRegisterCard = String(BASE_URL) + "/reg_card?tag=";

bool regCard = false;

void setup()
{
  Serial.begin(9600);

  // lcd begin
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print(" START SYSTEM");
  lcd.setCursor(3, 1);
  lcd.print("DOOR LOCK");
  delay(5000);
  lcd.clear();

  // relay off
  digitalWrite(RELAY, RELAY_OFF);

  // wifi setup
  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(WIFI_SSID, WIFI_PASS);

  // Begin WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(100);
    Serial.print(".");
    lcd.setCursor(0, 0);
    lcd.print("Connected to ...");
    lcd.setCursor(3, 1);
    lcd.print(WIFI_SSID);
  }

  Serial.println();
  Serial.print("Connected! IP Adress:");
  Serial.print(WiFi.localIP());

  SPI.begin();                                              // Initiate  SPI bus
  mfrc522.PCD_Init();                                       // Inisiasi MFRC522 card
  Serial.println("Approximate your card to the reader..."); // pesan untuk mentap kartu
  Serial.println();

  // setup telegram
  myBot.wifiConnect(WIFI_SSID, WIFI_PASS);
  myBot.setTelegramToken(TOKEN);

  if (myBot.testConnection())
  {
    Serial.println("Test Connection OK");
  }
  else
  {
    Serial.println("Test Connection Error");
  }

  // relay
  pinMode(RELAY,OUTPUT);
  digitalWrite(RELAY, HIGH);

//  showDisplay(" DEKATKAN KARTU ", "  UNTUK MEMBUKA ");
}

void loop()
{
  showDisplay(" DEKATKAN KARTU ", "  UNTUK MEMBUKA ");
  TBMessage msg;
  myBot.getNewMessage(msg);
  String strMsg = msg.text;
  strMsg.toUpperCase();
  
  if (!mfrc522.PICC_IsNewCardPresent())
  {
    Serial.println("ICC_IsNewCardPresent");
    if (strMsg.substring(0, 6) == "DAFTAR" && strMsg.indexOf(" ") == 6)
    {
      code_reg = strMsg.substring(strMsg.indexOf(" ") + 1);
      myBot.sendMessage(msg.sender.id, "DEKATKAN KARTU ANDA UNTUK MENDAFTAR");
      regCard = true;
      showDisplay("DEKATKAN KARTU", "UNTUK MENDAFTAR");
    }
    return;
  }

  if (!mfrc522.PICC_ReadCardSerial())
  {
    Serial.println("PICC_ReadCardSerial");
    return;
  }

  // menampilkan UID di serial Monitor
  Serial.print("UID tag :");
  String content = "";
  byte letter;

  // berfungsi untuk mengubah ID dari Kartu menjadi kode Hexa seperti 04 EA F0 2B
  for (byte i = 0; i < mfrc522.uid.size; i++)
  {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  content.toUpperCase(); // UID dijadikan Huruf besar semua
  content.trim();
  content.replace(" ", "%20");

  if (regCard == true)
  {
    Serial.println("KODE REG :" + code_reg);
    String response = getResponse("http://smart-gallon.000webhostapp.com/get_code_register");
    Serial.println("RESPONSE :" + response);
    Serial.println(strMsg.substring(strMsg.indexOf(" ") + 1) == response);

    if (code_reg == response)
    {
      String response = getResponse(String(urlRegisterCard + content + "&chat_id=" + String(msg.sender.id)));
      Serial.println(response);
      if (response == "success")
      {
        showDisplay("    SUKSES    ", "     TERDAFTAR      ");
        myBot.sendMessage(response.substring(response.indexOf("+") + 1).toInt(), "SUKSES TERDAFTAR");
        delay(2000);
      }
      else
      {
        Serial.println("TIDAK DAPAT MASUK");
        showDisplay("   TIDAK DAPAT  ", "     MASUK     ");
      }
    }

    regCard = false;
  }
  else
  {
    String response = getResponse(String(urlSaveHistory + content));
    Serial.println(response);
    Serial.println("RESULT : " + response.substring(0, response.indexOf("+")));
    if (response.substring(0, response.indexOf("+")) == "success")
    {
      showDisplay("    SILAHKAN    ", "     MASUK      ");
//      myBot.sendMessage(response.substring(response.indexOf("+") + 1).toInt(), "Anda telah masuk!");
      myBot.sendMessage(505217041, "Anda telah masuk!");
      Serial.println(response.substring(response.indexOf("+") + 1));
      Serial.println("SILAHKAN MASUK");
      digitalWrite(RELAY, LOW);
      delay(5000);
    }
    else
    {
      Serial.println("TIDAK DAPAT MASUK");
      showDisplay("   TIDAK DAPAT  ", "     MASUK     ");
    }
  }
  digitalWrite(RELAY, HIGH);
  delay(1000);
}
void showDisplay(String message1, String message2)
{
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print(message1);
  lcd.setCursor(0, 1);
  lcd.print(message2);
}
  
String getResponse(String url)
{
  if ((WiFiMulti.run() == WL_CONNECTED))
  {

    WiFiClient client;

    HTTPClient http;

    Serial.print("[HTTP] begin...\n");
    if (http.begin(client, url))
    { // HTTP

      Serial.print("[HTTP] GET...\n");
      // start connection and send HTTP header
      int httpCode = http.GET();

      // httpCode will be negative on error
      if (httpCode > 0)
      {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTP] GET... code: %d\n", httpCode);

        // file found at server
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY)
        {
          String payload = http.getString();
          return payload;
        }
        else
        {
          return "failed!";
        }
      }
      else
      {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
      }

      http.end();
    }
    else
    {
      Serial.printf("[HTTP} Unable to connect\n");
    }
  }
  delay(100);
}
