#include <SPI.h>
#include <MFRC522.h>
#define SS_PIN 2
#define RST_PIN 0
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.
#include <WiFiServer.h>
#include <WiFiClient.h>
#include <ESP8266WiFi.h>
#include <LiquidCrystal.h>
#define WIFI_BUFFER_SIZE 128
LiquidCrystal lcd(16, 5, 4, 15, 3, 1);
int buzzer = 10;
const char* ssid = "DIR Makeistan";
const char* password = "makerfall";
 
WiFiServer server(4000);
WiFiClient client;
 
int status = WL_IDLE_STATUS;
boolean ClientConnected = false;
boolean LastClientConnected = false;
char wifiBuffer[WIFI_BUFFER_SIZE];

void SetClientConnected(boolean flag)
{
 /* Only process this routine when the ClientConnected state has actually changed.
 Otherwise, return immediately. */
 if (flag != LastClientConnected)
 {
 ClientConnected = flag;
 LastClientConnected = ClientConnected;
 if (ClientConnected)
 {
 Serial.println("Client Connected");
 lcd.print("Client Connected");
 }
 else
 {
 Serial.println("Client Disconnected");
 lcd.print("Client Disctted");
 }
 }
}
 
boolean CheckClientConnection()
{
 /* If we have a running WiFiClient and there is a remote connection, just confirm the connection*/
 if (client && client.connected())
 {
 return true;
 }
 
 /* If we have a running WiFiClient but the remote has disconnected, disable WiFiClient and report no connection */
 if (client && !client.connected())
 {
 client.stop();
 return false;
 }
 
 /* At this point we are ready for a new remote connection. Create the WiFiClient and confirn the connection */
 if (server.hasClient())
 {
 if ((!client) || (!client.connected()))
 {
 if (client) client.stop();
 client = server.available();
 return true;
 }
 }
}
 
boolean GetClientData()
{
 /* If the remote connection has sent data, read the data and put it into the WiFiBuffer.
 Tell the main loop that data is available for processing. Otherwise, return immediately and
 report that no data is available. */
 if (client.available())
 {
 String c = client.readStringUntil('\r');
 client.flush();
 c.toCharArray(wifiBuffer, WIFI_BUFFER_SIZE);
 return true;
 }
 else
 {
 return false;
 }
}
 
void ProcessClientData(void)
{
 /* This is a trivial routine to process data from remote socket. It simply repeats the data to the
 Serial port, and sends an "OK" confirmation back to the remote socket. */
 Serial.print("Client Data Received: ");
 Serial.println(wifiBuffer);
 lcd.setCursor(0,0);   
 
 if (client)
 {
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
    GetClientData();
    lcd.clear();
    if(wifiBuffer == "not registered"){
      lcd.setCursor(0,0);
      lcd.print("Please register");
      lcd.setCursor(0,1);
      lcd.print("inside Makeistan");
      for(int i = 0;i<10;i++){
        tone(buzzer,4000);
        delay(100);
        noTone(buzzer);
        delay(100);
      }
    }
    else if(wifiBuffer == "new registration"){
      lcd.setCursor(0,0);
      lcd.print("You are now");
      lcd.setCursor(0,1);
      lcd.print("registered!");
      tone(buzzer,2000);
      delay(1000);
      noTone(buzzer);
    }
    else{
      lcd.setCursor(0,0);
      lcd.print("Welcome");
      lcd.setCursor(0,1);
      lcd.print(wifiBuffer);
      tone(buzzer,3000);
      delay(1000);
      noTone(buzzer);
    }
    delay(5000);
 }

}
 
void setup() 
{
   Serial.begin(9600);   // Initiate a serial communication
   noTone(buzzer);
   lcd.begin(16,2);
   delay(100);
   SPI.begin();      // Initiate  SPI bus
   mfrc522.PCD_Init();   // Initiate MFRC522
   Serial.println("Approximate your card to the reader...");
   Serial.println();
   /* Log NodeMCU on to LAN. Provide IP Address over Serial port */
   WiFi.begin(ssid, password);
   while (WiFi.status() != WL_CONNECTED) {
   delay(250);
   }
   Serial.print("WiFi Connected at ");
   lcd.print("WiFi Connected:");
   Serial.println(WiFi.localIP());
   lcd.setCursor(0,1);
   lcd.println(WiFi.localIP());
   /* Start Server */
   server.begin();
   delay(1000);
   Serial.println("Server Ready");
   lcd.setCursor(0,0);
   lcd.clear();
   lcd.print("Server Ready");
   delay(1000);
}
void loop() 
{
  SetClientConnected(CheckClientConnection());
 
  if (ClientConnected || GetClientData())
  {
    ProcessClientData();
      // Look for new cards
  }
 delay(200);
 lcd.clear();
 lcd.setCursor(0,0);
} 
 

