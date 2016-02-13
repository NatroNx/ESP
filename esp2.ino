#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <Arduino.h>
#include <WebSocketsServer.h>
#include <Hash.h>
#include "RTClib.h"
#include <Wire.h>
#include <WiFiUdp.h>



#define BLUEPIN 13

#define SSID_ME "Wlan07"
#define PW_ME "wlan_01!_2005!grojer_.."
#define HOST_ME "ESP8266"



unsigned int localPort = 2390;      // local port to listen for UDP packets
IPAddress timeServerIP; // time.nist.gov NTP server address
const char* ntpServerName = "time.nist.gov";
const int NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message
byte packetBuffer[ NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets
// A UDP instance to let us send and receive packets over UDP
WiFiUDP udp;








IPAddress ip(10,0,0,200);  //Node static IP
IPAddress gateway(10,0,0,138);
IPAddress subnet(255,255,255,0);


WebSocketsServer webSocket = WebSocketsServer(9000);
ESP8266WebServer httpServer(80);
ESP8266HTTPUpdateServer httpUpdater;
const char* host = HOST_ME;
const char* ssid     = SSID_ME;
const char* password = PW_ME;
unsigned long lastTimeHost;
unsigned long lastTimeSent;
unsigned long lastTimeNTPUpdate;
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
DateTime ntpDateTime;


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
  udp.begin(localPort);     //ntp 
  lastTimeNTPUpdate=millis();
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


      if (millis()-lastTimeNTPUpdate>60000 * 1 )   // alle 1 Minuten NTP Update
{
 Serial.println(lastTimeNTPUpdate);
  Serial.println(millis());
 
  //get a random server from the pool
  WiFi.hostByName(ntpServerName, timeServerIP); 
  sendNTPpacket(timeServerIP); // send an NTP packet to a time server
  // wait to see if a reply is available

  // wait to see if a reply is available
  delay(3000);
  
  int cb = udp.parsePacket();
  if (!cb) {
    Serial.println("no packet yet");
  }

 else {
    Serial.print("packet received, length=");
    Serial.println(cb);
    // We've received a packet, read the data from it
    udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer

    //the timestamp starts at byte 40 of the received packet and is four bytes,
    // or two words, long. First, esxtract the two words:

    unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
    unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
    // combine the four bytes (two words) into a long integer
    // this is NTP time (seconds since Jan 1 1900):
    unsigned long secsSince1900 = highWord << 16 | lowWord;
    Serial.print("Unixtime: " );
    Serial.println(secsSince1900-2208988800UL);
    ntpDateTime=secsSince1900-2208988800UL+3600;  //creating unixtime +1 Hour (timezone)
    sendCommand("put|ntpdatetime_" + String(ntpDateTime.unixtime()) + " \n");  //send the data to mega
    lastTimeNTPUpdate=millis();
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
       //___________ DONT PARSE DATETIME - datetime comes from NTP Source____________________________________use this to parsDateTime in Seconds unixtimne!
/*      if(part1.substring(0, part1.indexOf("_")).equalsIgnoreCase("testDateTime"))
      {
        char arr[20];
        (part1.substring(part1.indexOf("_")+1,part1.length())).toCharArray(arr, sizeof(arr));
       testDateTime=atol(arr);
        
      } 
      */
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
      if(part1.substring(0, part1.indexOf("_")).equalsIgnoreCase("ntpDateTime"))
      {       
        sendCommand(String(ntpDateTime.unixtime()));
        // sendCommand(String(testDateTime.unixtime()));
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
   char charBuf[150];
   sendThis.toCharArray(charBuf, 150);
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
      else if (text.substring(0, text.indexOf("|")).equalsIgnoreCase("viewAllData"))
      {
      webSocket.broadcastTXT(testString);
      webSocket.broadcastTXT(String(testInt));
      webSocket.broadcastTXT(String(testFloat));
      webSocket.broadcastTXT(String(testByte));
      webSocket.broadcastTXT(String(testBoolean));
      webSocket.broadcastTXT(String(ntpDateTime.hour(), DEC));
      webSocket.broadcastTXT(String(':'));
      webSocket.broadcastTXT(String(ntpDateTime.minute(), DEC));
      webSocket.broadcastTXT(String(':'));
      webSocket.broadcastTXT(String(ntpDateTime.second(), DEC));
      webSocket.broadcastTXT(String(' '));
      webSocket.broadcastTXT(String(ntpDateTime.day(), DEC));
      webSocket.broadcastTXT(String('.'));
      webSocket.broadcastTXT(String(ntpDateTime.month(), DEC));
      webSocket.broadcastTXT(String('.'));
      webSocket.broadcastTXT(String(ntpDateTime.year(), DEC));      
      }
      else if (text.substring(0, text.indexOf("|")).equalsIgnoreCase("GetData"))
      {
      getAllData();
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


void getAllData()
{
  sendCommand("get|teststring|testint|testfloat|testbyte|testboolean|ntpDateTime");
              
}


// HTTP updater connection
void HTTPUpdateConnect() {
  httpUpdater.setup(&httpServer);
  httpServer.begin();
  Serial.print("Connect to ");
  Serial.println(WiFi.localIP());

}





// send an NTP request to the time server at the given address
unsigned long sendNTPpacket(IPAddress& address)
{
  Serial.println("sending NTP packet...");
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;

  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  udp.beginPacket(address, 123); //NTP requests are to port 123
  udp.write(packetBuffer, NTP_PACKET_SIZE);
  udp.endPacket();
}





