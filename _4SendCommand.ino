



void sendCommand(String variable, String sendCom)
{String sendThis = "<toMega|" + variable + "_" + sendCom + "|" ; 
 int CheckSum = sendThis.length()-1;
 sendThis+='*';
 if(CheckSum<10) {sendThis+="00";}
 else if (CheckSum<100) {sendThis+=0;}                
 sendThis+=CheckSum;    
 sendThis+=">";            
 char charBuf[250];
  sendThis.toCharArray(charBuf, 250);
  Serial.write(charBuf);
# if debug
  { Serial.print(F("DebugLine: "));
    Serial.println(charBuf);
  }
#endif
}



void sendSerial(String sendCom)
{webSockWait=millis();
  String sendThis = "<"+ sendCom + "|" ; 
 int CheckSum = sendThis.length()-1;
 sendThis+='*';
 if(CheckSum<10) {sendThis+="00";}
 else if (CheckSum<100) {sendThis+="0";}                
 sendThis+=CheckSum;    
 sendThis+=">";            
 char charBuf[250];
  sendThis.toCharArray(charBuf, 250);
  Serial.write(charBuf);
  


  
# if debug
  {
    Serial.print(charBuf);
  }
#endif
}


void webSocketUpdate()
{
      webSocket.broadcastTXT(
        "parse|pH_"+ String(PhWert)      +  
        "|tE_"+String(Temp)        +
        "|tV_"+String(TVModeState)        +
        "|cI_"+String(cleaningInProcess)   +
        "|mO_"+ String(manualOverride)      +
        "|mM_"+String(MoonModeState)        +
        "|cP_"+String(calculatedPWM)        +
        "|cR_"+String(calculatedRed)   +
        "|cG_"+ String(calculatedGreen)      +
        "|cB_"+String(calculatedBlue)        +
        "|p1_"+String(pump1Value)   +
        "|p2_"+String(pump2Value)        +
        "|lV_"+String(light230Value)   +
        "|l1_"+ String(light1Value)      +
        "|l2_"+String(light2Value)        +
        "|cO_"+String(co2Value)   +
        "|hV_"+ String(heaterValue)      +
        "|cV_"+ String(coolValue)      +
        "|nO_"+String(now.unixtime())        +
        "|");
}



