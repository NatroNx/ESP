#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <Arduino.h>
#include <WebSocketsServer.h>
#include <Hash.h>
#include "RTClib.h"
#include <Wire.h>



#define BLUEPIN 13

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
String testString = "ESP";
int testInt = 1;
float testFloat = 2;
byte testByte = 3;
boolean testBoolean = false;
DateTime testDateTime;

String StringFromSerial;
unsigned long sent = millis();
int i = 0;
int callme=0;




void setup()
{
  Serial.begin(115200);     
  Serial.println("Serial Monitor Connected");  
  pinMode(BLUEPIN, OUTPUT);
  analogWriteFreq(200);
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
{ 
String part1;
 // part1 = com.substring(0, com.indexOf("|"));
  if(com.substring(0, com.indexOf("|")).equalsIgnoreCase("Put"))
    {
    for(int i=com.indexOf("|"); i<com.lastIndexOf("|"); i=com.indexOf("|",i+1))
     {String part1 = com.substring(i+1, com.indexOf("|",i+1));
      //____________________________________________________________________________________________________use this to parseStrings
      if(part1.substring(0, part1.indexOf("_")).equalsIgnoreCase("teststring"))
      {
        testString=part1.substring(part1.indexOf("_")+1,part1.length());
        
      }
      //____________________________________________________________________________________________________use this to parseInts
      if(part1.substring(0, part1.indexOf("_")).equalsIgnoreCase("testInt"))
      {
        testInt=(part1.substring(part1.indexOf("_")+1,part1.length())).toInt();
        
      }      
      //____________________________________________________________________________________________________use this to parseFloats
      if(part1.substring(0, part1.indexOf("_")).equalsIgnoreCase("testFloat"))
      {
        testFloat=(part1.substring(part1.indexOf("_")+1,part1.length())).toFloat();
        
      }    
       //____________________________________________________________________________________________________use this to parseByte
      if(part1.substring(0, part1.indexOf("_")).equalsIgnoreCase("testByte"))
      {
        testByte=(part1.substring(part1.indexOf("_")+1,part1.length())).toInt();
        
      }  
       //____________________________________________________________________________________________________use this to parseBoolean
      if(part1.substring(0, part1.indexOf("_")).equalsIgnoreCase("testBoolean"))
      {
        testBoolean=(part1.substring(part1.indexOf("_")+1,part1.length())).toInt();
        
      }  
       //____________________________________________________________________________________________________use this to parseByte
      if(part1.substring(0, part1.indexOf("_")).equalsIgnoreCase("testBoolean"))
      {
        testBoolean=(part1.substring(part1.indexOf("_")+1,part1.length())).toInt();
        
      }  
       //____________________________________________________________________________________________________use this to parsDateTime in Seconds unixtimne!
      if(part1.substring(0, part1.indexOf("_")).equalsIgnoreCase("testDateTime"))
      {
        char arr[12];
        (part1.substring(part1.indexOf("_")+1,part1.length())).toCharArray(arr, sizeof(arr));
       testDateTime=atol(arr);
        
      } 
      else
      {
        sendCommand(String(com));
      }
       
    }
    }
  else  if(com.substring(0, com.indexOf("|")).equalsIgnoreCase("Get"))
    {
    for(int i=com.indexOf("|"); i<com.lastIndexOf("|"); i=com.indexOf("|",i+1))
     {String part1 = com.substring(i+1, com.indexOf("|",i+1));
      //____________________________________________________________________________________________________use this to parseStrings
      if(part1.substring(0, part1.indexOf("_")).equalsIgnoreCase("teststring"))
      {
        sendCommand(testString);
      }
      //____________________________________________________________________________________________________use this to parseInts
      if(part1.substring(0, part1.indexOf("_")).equalsIgnoreCase("testInt"))
      {
        sendCommand(String(testInt));
      }      
      //____________________________________________________________________________________________________use this to parseFloats
      if(part1.substring(0, part1.indexOf("_")).equalsIgnoreCase("testFloat"))
      {
        sendCommand(String(testFloat));
      }    
       //____________________________________________________________________________________________________use this to parseByte
      if(part1.substring(0, part1.indexOf("_")).equalsIgnoreCase("testByte"))
      {
        sendCommand(String(testByte));
      }  
       //____________________________________________________________________________________________________use this to parseBoolean
      if(part1.substring(0, part1.indexOf("_")).equalsIgnoreCase("testBoolean"))
      {
        sendCommand(String(testBoolean));
      }  
       //____________________________________________________________________________________________________use this to parsDateTime in Seconds unixtimne!
      if(part1.substring(0, part1.indexOf("_")).equalsIgnoreCase("testDateTime"))
      {        sendCommand(String(testDateTime.unixtime()));
      }  
    } 
    }

  else
   {
    //Serial.println(com);
   }
  
}







void sendCommand(String sendCom)
{
  String sendThis = sendCom + "| \n" ;
   char charBuf[100];
   sendThis.toCharArray(charBuf, 100);
   Serial.println(charBuf);
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
          else if (text.startsWith("r")) {
          String zVal = (text.substring(text.indexOf("y") + 1, text.length()));
          //analogWrite(BLUEPIN, zInt);
          webSocket.broadcastTXT(testString);
          webSocket.broadcastTXT(String(testInt));
          webSocket.broadcastTXT(String(testFloat));
          webSocket.broadcastTXT(String(testByte));
          webSocket.broadcastTXT(String(testBoolean));

        webSocket.broadcastTXT(String(testDateTime.hour(), DEC));
        webSocket.broadcastTXT(String(':'));
        webSocket.broadcastTXT(String(testDateTime.minute(), DEC));
        webSocket.broadcastTXT(String(':'));
        webSocket.broadcastTXT(String(testDateTime.second(), DEC));
        webSocket.broadcastTXT(String(' '));
        webSocket.broadcastTXT(String(testDateTime.day(), DEC));
        webSocket.broadcastTXT(String('.'));
        webSocket.broadcastTXT(String(testDateTime.month(), DEC));
        webSocket.broadcastTXT(String('.'));
        webSocket.broadcastTXT(String(testDateTime.year(), DEC));
          
        }
      else
      {
        sendCommand(text);
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







