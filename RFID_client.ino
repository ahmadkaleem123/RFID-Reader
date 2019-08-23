#include <SPI.h>
#include <MFRC522.h>
#define SS_PIN 2
#define RST_PIN 0
MFRC522 mfrc522(SS_PIN, RST_PIN);  
#include <WiFiServer.h>
#include <WiFiClient.h>
#include <ESP8266WiFi.h>
#include <LiquidCrystal.h>
LiquidCrystal lcd(16, 5, 4, 15, 3, 1);
int buzzer = 10;
const char* ssid = "DIR Makeistan";
const char* password = "makerfall";
const char* server = "api.thingspeak.com";
WiFiClient client;
void setup() {
   Serial.begin(115200);
   noTone(buzzer);
   lcd.begin(16,2);
   delay(100);
   SPI.begin();
   mfrc522.PCD_Init();
   WiFi.begin(ssid, password);
   while (WiFi.status() != WL_CONNECTED) {
    delay(250);
   }
   Serial.print("WiFi Connected at ");
   lcd.print("WiFi Connected:");
   Serial.println(WiFi.localIP());
   lcd.setCursor(0,1);
   lcd.println(WiFi.localIP());
}

void loop() {
  if(client.connect(server,80)) {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Scan Card");
    if ( ! mfrc522.PICC_IsNewCardPresent()) 
    {
      return;
    }
    // Select one of the cards
    if ( ! mfrc522.PICC_ReadCardSerial()) 
    {
      return;
    }
    //Show UID on serial monitor
    Serial.print("UID tag :");
    //lcd.print("UID tag :");
    String content= "";
    byte letter;
    for (byte i = 0; i < mfrc522.uid.size; i++) 
    {
       Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
       Serial.print(mfrc522.uid.uidByte[i], HEX);
       content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
       content.concat(String(mfrc522.uid.uidByte[i], HEX));
    }
    Serial.println();
    Serial.print("Message : ");
    content.toUpperCase(); // content is the variable which saves the UID.    This can be sent to the raspberry pi.
    //lcd.setCursor(0,1);
    //lcd.print(content.substring(1));
    client.println(content.substring(1)); // Sends data to the raspberry pi
    while(client.available()){
      char c = client.read();
      Serial.print(c);
      lcd.clear();
      lcd.print(c);
    }
  }
  client.stop();
  delay(1000);
}
