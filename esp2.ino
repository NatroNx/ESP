#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <Arduino.h>
#include <WebSocketsServer.h>
#include <Hash.h>




#define BLUEPIN 12

#define SSID_ME "Wlan07"
#define PW_ME "wlan_01!_2005!grojer_.."
#define HOST_ME "ESP8266"
IPAddress ip(10,0,0,200);  //Node static IP
IPAddress gateway(10,0,0,138);
IPAddress subnet(255,255,255,0);


WebSocketsServer webSocket = WebSocketsServer(81);
ESP8266WebServer httpServer(80);
ESP8266HTTPUpdateServer httpUpdater;
const char* host = HOST_ME;
const char* ssid     = SSID_ME;
const char* password = PW_ME;
unsigned long lastTimeHost;
unsigned long lastTimeSent;
String text="z10";
int zInt;
int zOld;


/*     
 For more details see: http://projectsfromtech.blogspot.com/
 

 Read char value from Serial Monitor
 Display that value on the Serial Monitor
 */

String StringFromSerial;
unsigned long sent = millis();
int i = 0;
int callme=0;




void setup()
{
  Serial.begin(115200);     
  Serial.println("Serial Monitor Connected");  
  pinMode(BLUEPIN, OUTPUT);
  WifiConnect();
  WebSocketConnect();
  HTTPUpdateConnect();
}

void loop() 
{  if (WiFi.status() != WL_CONNECTED)
  {
    WifiConnect();
    WebSocketConnect();
  }
  else 
  {
    webSocket.loop();
    if (millis() - lastTimeHost > 1)
    {
      httpServer.handleClient();
      lastTimeHost = millis();
    }

  }
  
  if(millis()-sent>100)
    {sent=millis();
     String tool = "Test" + String(i, DEC);
     //analogWrite(BLUEPIN, 255);
     //Serial.println(i);
     //sendCommand(tool);
     i++;
    }
    
    if(Serial.available())
    {
        char charFromSerial = Serial.read();
        if (charFromSerial == '\n')
          { parseCommand(StringFromSerial);
            StringFromSerial="";
          }
          else
          {
            StringFromSerial += charFromSerial;
          }
      }   
}

void parseCommand(String com)
{ String part1;
part1 = com.substring(0, com.indexOf("|"));
  if(part1.equalsIgnoreCase("ack"))
  {i=0;
   //sendCommand("GiveMillis");
  // sendCommand("How long will  ESP8266 to an Arno Mega via Serial line?");
   //sendCommand("GiveMillis");
  }  
}




void sendCommand(String sendCom)
{
  String sendThis = sendCom + "| \n" ;
   char charBuf[100];
   sendThis.toCharArray(charBuf, 100);
   Serial.write(charBuf);
}



// WebSOcket Events
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length)
{
  switch (type) {
    case WStype_DISCONNECTED:
      break;


    case WStype_CONNECTED:
      {
        IPAddress ip = webSocket.remoteIP(num);
        Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\r\n", num, ip[0], ip[1], ip[2], ip[3], payload);
        // Send the current LED status
 
          webSocket.sendTXT(num, text);
      }
      
    
      break;


      
    case WStype_TEXT:
      {
        text = String((char *) &payload[0]);

        if (text.startsWith("u")) {
          String zVal = (text.substring(text.indexOf("u") + 1, text.length()));
          int zInt = zVal.toInt();
          analogWrite(BLUEPIN, zInt);
          webSocket.broadcastTXT(text);
        }

        else if (text.startsWith("z")) {
          String zVal = (text.substring(text.indexOf("z") + 1, text.length()));
          int zInt = zVal.toInt();
          Serial.println(zInt);
          analogWrite(BLUEPIN, zInt);
        }
      else
      {
        //sendCommand(text);
      }

        

       }
      break;

    case WStype_BIN:
      hexdump(payload, length);
      // echo data back to browser
      webSocket.sendBIN(num, payload, length);
      break;
  }
}


// Wifi Connection
void WifiConnect() {

 // WiFi.mode(WIFI_AP_STA);
  WiFi.begin(ssid, password);
  WiFi.config(ip, gateway, subnet);

  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
  }
  Serial.println(WiFi.localIP());

}

// WebSocket Connection
void WebSocketConnect() {
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
}



// HTTP updater connection
void HTTPUpdateConnect() {
  httpUpdater.setup(&httpServer);
  httpServer.begin();
  Serial.print("Connect to ");
  Serial.println(WiFi.localIP());

}







