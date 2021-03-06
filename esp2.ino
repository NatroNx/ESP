//v.1.0.0 - 08.03.2016 -   init 
//v.1.0.1 - 08.03.2016 - minor fixes
//v.1.0.2 - 10.03.2016 - sendSerial with and without Feedback  - choose your weapons
//v 1.0.3 - 25.03.2016 - fixed the bug that Information received over serial wasn't complete
//v 1.0.4 - 28.03.2016 - changed ntp to europe (for DST), added automatic DST switching
//v 1.0.5 - 28.10.2016 - implementing first TempCurve Tests


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
#include <pgmspace.h>


#define BLUEPIN 13

#define SSID_ME "Wlan07"
#define PW_ME "1."
#define HOST_ME "ESP8266"



unsigned int localPort = 2390;      // local port to listen for UDP packets
IPAddress timeServerIP; // time.nist.gov NTP server address
const char* ntpServerName = "europe.pool.ntp.org";
const int NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message
byte packetBuffer[ NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets
// A UDP instance to let us send and receive packets over UDP
WiFiUDP udp;

#define debug 1




const byte numChars = 255;
char receivedChars[numChars];
static byte ndx = 0;
boolean newData = false;

boolean webSockUpdate = false;
unsigned long webSockWait = 0;






enum {tPhWert, tTemp, tcalculatedPWM, tcalculatedRed, tcalculatedGreen, tcalculatedBlue, tTVModeState, tcleaningInProcess,
      tmanualOverride, tMoonModeState, tpump1Value, tpump2Value, tlight230Value, tlight1Value,
      tlight2Value, tco2Value, theaterValue, tdPump1Value, tdPump2Value, tdPump3Value, tcoolValue, tnow, tprocess
     };

const char PhWert_Char[] PROGMEM    = "pH";
const char Temp_Char[] PROGMEM    = "tE";
const char calculatedPWM_Char[] PROGMEM    = "cP";
const char calculatedRed_Char[] PROGMEM    = "cR";
const char calculatedGreen_Char[] PROGMEM    = "cG";
const char calculatedBlue_Char[] PROGMEM    = "cB";
const char TVModeState_Char[] PROGMEM    = "tV";
const char cleaningInProcess_Char[] PROGMEM    = "cI";
const char manualOverride_Char[] PROGMEM    = "mO";
const char MoonModeState_Char[] PROGMEM    = "mM";
const char pump1Value_Char[] PROGMEM    = "p1";
const char pump2Value_Char[] PROGMEM    = "p2";
const char light230Value_Char[] PROGMEM    = "lV";
const char light1Value_Char[] PROGMEM    = "l1";
const char light2Value_Char[] PROGMEM    = "l2";
const char co2Value_Char[] PROGMEM    = "cO";
const char heaterValue_Char[] PROGMEM    = "hV";
const char dPump1Value_Char[] PROGMEM    = "d1";
const char dPump2Value_Char[] PROGMEM    = "d2";
const char dPump3Value_Char[] PROGMEM    = "d3";
const char coolValue_Char[] PROGMEM    = "cV";
const char now_Char[] PROGMEM    = "nO";
const char process_Char[] PROGMEM    = "pR";

PGM_P const Char_table[] PROGMEM  =
{ PhWert_Char, Temp_Char, calculatedPWM_Char, calculatedRed_Char, calculatedGreen_Char, calculatedBlue_Char, TVModeState_Char,
  cleaningInProcess_Char, manualOverride_Char, MoonModeState_Char, pump1Value_Char, pump2Value_Char,
  light230Value_Char, light1Value_Char, light2Value_Char, co2Value_Char, heaterValue_Char,
  dPump1Value_Char, dPump2Value_Char, dPump3Value_Char, coolValue_Char, now_Char, process_Char
};

int charCount = sizeof(Char_table) / sizeof(Char_table[0]);







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

float TempValues[96];
int Co2Values[96];
float PHValues[96];



// time to get live
DateTime now;
//modes
boolean TVModeState = false;
boolean cleaningInProcess = false;
boolean manualOverride = false;
boolean MoonModeState = false;
//lights
float calculatedPWM = 100;   
float calculatedRed = 1;
float calculatedGreen = 2;
float calculatedBlue = 3;
//Relais
boolean pump1Value = false;
boolean pump2Value = false;
boolean light230Value = true;
boolean light1Value = true;
boolean light2Value = true;
boolean co2Value = true;
boolean heaterValue = false;
boolean dPump1Value = true;
boolean dPump2Value = true;
boolean dPump3Value = true;
boolean coolValue = true;
//values to monitor
float PhWert = 7.01; //sting to float to calculate with it
float Temp = 11.11;  






String StringFromSerial;
unsigned long sent = millis();
int i = 0;
int callme=0;




void setup()
{
  Serial.begin(9600);     
  Serial.println("Serial Monitor Connected");  
  pinMode(BLUEPIN, OUTPUT);
  analogWriteFreq(200);
  WifiConnect();
  WebSocketConnect();
  HTTPUpdateConnect();
  udp.begin(localPort);     //ntp 
  lastTimeNTPUpdate=0;

  for (int i = 0; i < 96; i++)
  {
    float rndT=random(250, 270);
    TempValues[i] = rndT/10;
    rndT=random(690, 710);
    PHValues[i] = rndT/100;
    rndT=random(0, 19);
    Co2Values[i] = rndT/10;
    
  }
  

  
  
}


  void recvWithStartEndMarkers() {

 
  static boolean recvInProgress = false;
    char startMarker = '<';
    char endMarker = '>';
    char rc;
   
 
 // if (Serial.available() > 0) {
    while (Serial.available() > 0 && newData == false) 
    {
        rc = Serial.read();

        if (recvInProgress == true) {
            if (rc != endMarker) {
                receivedChars[ndx] = rc;
                ndx++;
                if (ndx >= numChars) {
                    ndx = numChars - 1;
                }
            }
            else {
                
                receivedChars[ndx] = '\0'; // terminate the string
                recvInProgress = false;
                ndx=0;
                newData = true;
            }
        }

        else if (rc == startMarker) {
            recvInProgress = true;
        }
    }

}









  
void useNewData() {

    if (newData == true) {

                StringFromSerial=receivedChars;
                
                parseCommand(StringFromSerial);
              // sendSerial("dbgES: " + StringFromSerial);
                StringFromSerial="";
                ndx = 0;
                newData = false;
              }
}





void loop() 
{  
  if (WiFi.status() != WL_CONNECTED)
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

   recvWithStartEndMarkers();
    useNewData();
    
    


 if (millis()-lastTimeNTPUpdate>60000 * 360||  lastTimeNTPUpdate==0 )   // on bootup and every 6 hours NTP Update
{
  //get a random server from the pool
  WiFi.hostByName(ntpServerName, timeServerIP); 
  sendNTPpacket(timeServerIP); // send an NTP packet to a time server
  // wait to see if a reply is available

  // wait to see if a reply is available
  delay(3000);
  
  int cb = udp.parsePacket();
  if (!cb) {
# if debug
{
       Serial.println("no packet yet");
}
#endif 
   
  }

 else {
# if debug
{    Serial.print("packet received, length=");
    Serial.println(cb);
 }
#endif 
 
    // We've received a packet, read the data from it
    udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer

    //the timestamp starts at byte 40 of the received packet and is four bytes,
    // or two words, long. First, esxtract the two words:

    unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
    unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
    // combine the four bytes (two words) into a long integer
    // this is NTP time (seconds since Jan 1 1900):
    unsigned long secsSince1900 = highWord << 16 | lowWord;
    # if debug
    Serial.print("Unixtime: " );
    Serial.println(secsSince1900-2208988800UL);
 #endif    
    now=secsSince1900-2208988800UL+3600;  //creating unixtime +1 Hour (for CET)
    if (IsDst(now.day(), now.month(), now.dayOfTheWeek()))  //if Summertime +1 hour
    {now=now+3600;
    }
    sendCommand("nO", String(now.unixtime()) + "|");  //send the data to mega
    lastTimeNTPUpdate=millis();
 }

}




if (webSockUpdate)
{
  if (millis()-webSockWait>500)
  {webSockWait=millis();
   webSockUpdate=false;
   webSocketUpdate();
   
  }
}

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
 
      }
      
    
      break;


      
    case WStype_TEXT:
      {
        text = String((char *) &payload[0]);

     if (text.substring(0, text.indexOf("|")).equalsIgnoreCase("viewAllData"))
      {

  # if debug
      webSocket.broadcastTXT(String(now.hour(), DEC) +  String(':')  +    String(now.minute(), DEC) + String(':') + String(now.second(), DEC) + String(' ')  + String(now.day(), DEC)+   String('.') +   String(now.month(), DEC) +    String('.') + String(now.year(), DEC)  );
      webSocket.broadcastTXT("PhWert: " + String(PhWert));
      webSocket.broadcastTXT("Temp: " + String(Temp));
      webSocket.broadcastTXT("TVModeState: " + String(TVModeState));
      webSocket.broadcastTXT("cleaningInProcess: " + String(cleaningInProcess));
      webSocket.broadcastTXT("manualOverride: " + String(manualOverride));
      webSocket.broadcastTXT("MoonModeState: " + String(MoonModeState));
      webSocket.broadcastTXT("calculatedPWM: " + String(calculatedPWM));
      webSocket.broadcastTXT("calculatedRed: " + String(calculatedRed));
      webSocket.broadcastTXT("calculatedGreen: " + String(calculatedGreen));
      webSocket.broadcastTXT("calculatedBlue: " + String(calculatedBlue));
      webSocket.broadcastTXT("pump1Value: " + String(pump1Value));
      webSocket.broadcastTXT("pump2Value: " + String(pump2Value));
      webSocket.broadcastTXT("light230Value: " + String(light230Value));
      webSocket.broadcastTXT("light1Value: " + String(light1Value));
      webSocket.broadcastTXT("light2Value: " + String(light2Value));
      webSocket.broadcastTXT("co2Value: " + String(co2Value));
      webSocket.broadcastTXT("heaterValue: " + String(heaterValue));
      webSocket.broadcastTXT("dPump1Value: " + String(dPump1Value));
      webSocket.broadcastTXT("dPump2Value: " + String(dPump2Value));
      webSocket.broadcastTXT("dPump3Value: " + String(dPump3Value));
      webSocket.broadcastTXT("coolValue: " + String(coolValue));
      #endif
      //webSockWait=millis();
      //webSockUpdate=true;
    


      
      }
      else if (text.substring(0, text.indexOf("|")).equalsIgnoreCase("GetData"))
      {
      getAllData();
      //webSockWait=millis();
      //webSockUpdate=true;

      }

      else if (text.equalsIgnoreCase("getTempArray"))
      { String TempString=String(TempValues[0]);
        String Co2String=String(Co2Values[0]);
        String PHString=String(PHValues[0]);
        
        for (int i=1; i<96; i++)
        {
        PHString=PHString+";"+String(PHValues[i]);
        TempString=TempString+";"+String(TempValues[i]);
        Co2String=Co2String+";"+String(Co2Values[i]);       
        }
        webSocket.broadcastTXT("parse|tS_"+TempString+"|");
        webSocket.broadcastTXT("parse|phS_"+PHString+"|");
        webSocket.broadcastTXT("parse|cS_"+Co2String+"|");
        
      }
      else
      {
        sendSerial(text);
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
  sendSerial("updateMe");
              
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
# if debug
  {
    Serial.println("sending NTP packet...");
  }
  #endif 
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




 boolean IsDst(int day, int month, int dow)
    {
        if (month < 3 || month > 10)  return false; 
        if (month > 3 && month < 10)  return true; 

        int previousSunday = day - dow;

        if (month == 3) return previousSunday >= 25;
        if (month == 10) return previousSunday < 25;

        return false; // this line never gonna happend
    }



