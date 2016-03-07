
void parseCommand(String com)
{ 
  int sentChecksum=(com.substring(com.indexOf("*")+1, com.length())).toInt();
  int calculatedCheckSum=com.indexOf("*");
 
  if(sentChecksum==calculatedCheckSum)
  {String part1;
  // part1 = com.substring(0, com.indexOf("|"));
  if (com.substring(0, com.indexOf("|")).equalsIgnoreCase("toESP"))
  {
    for (int i = com.indexOf("|"); i < com.lastIndexOf("|"); i = com.indexOf("|", i + 1))
    { String part1 = com.substring(i + 1, com.indexOf("|", i + 1));
      int cmdID = findCommand(part1.substring(0, part1.indexOf(F("_"))).c_str());
 
      switch (cmdID)
      { case tPhWert : {PhWert=(part1.substring(part1.indexOf(F("_"))+1,part1.length())).toFloat();  break;}
        case tTemp : {Temp=(part1.substring(part1.indexOf(F("_"))+1,part1.length())).toFloat();    break;}
        case tcalculatedPWM : {calculatedPWM = (part1.substring(part1.indexOf(F("_")) + 1, part1.length())).toFloat();  break;}
        case tcalculatedRed : {calculatedRed = (part1.substring(part1.indexOf(F("_")) + 1, part1.length())).toFloat();   break;}
        case tcalculatedGreen : { calculatedGreen = (part1.substring(part1.indexOf(F("_")) + 1, part1.length())).toFloat();   break;}
        case tcalculatedBlue : { calculatedBlue = (part1.substring(part1.indexOf(F("_")) + 1, part1.length())).toFloat();        break;}
        case tTVModeState : { TVModeState = (part1.substring(part1.indexOf(F("_")) + 1, part1.length())).toInt();   break;}
        case tcleaningInProcess : {cleaningInProcess = (part1.substring(part1.indexOf(F("_")) + 1, part1.length())).toInt();       break;}
        case tmanualOverride : {manualOverride = (part1.substring(part1.indexOf(F("_")) + 1, part1.length())).toInt();      break;}
        case tMoonModeState : {MoonModeState = (part1.substring(part1.indexOf(F("_")) + 1, part1.length())).toInt();       break;}
        case tpump1Value : {pump1Value = (part1.substring(part1.indexOf(F("_")) + 1, part1.length())).toInt();  break;}
        case tpump2Value : {pump2Value = (part1.substring(part1.indexOf(F("_")) + 1, part1.length())).toInt();       break;}
        case tlight230Value : {light230Value = (part1.substring(part1.indexOf(F("_")) + 1, part1.length())).toInt();                             break;}
        case tlight1Value : {light1Value = (part1.substring(part1.indexOf(F("_")) + 1, part1.length())).toInt();        break;}
        case tlight2Value : {light2Value = (part1.substring(part1.indexOf(F("_")) + 1, part1.length())).toInt();   break;}
        case tco2Value : {co2Value = (part1.substring(part1.indexOf(F("_")) + 1, part1.length())).toInt();     break;}
        case theaterValue : {heaterValue = (part1.substring(part1.indexOf(F("_")) + 1, part1.length())).toInt();      break;}
        //case tdPump1Value : {dPump1Value = (part1.substring(part1.indexOf(F("_")) + 1, part1.length())).toInt();  break;}
       // case tdPump2Value : {dPump2Value = (part1.substring(part1.indexOf(F("_")) + 1, part1.length())).toInt();  break;}
        //case tdPump3Value : {dPump3Value = (part1.substring(part1.indexOf(F("_")) + 1, part1.length())).toInt();       break;}
        case tnow : { now=1+(part1.substring(part1.indexOf(F("_")) + 1, part1.length())).toInt();  break;}
        case tcoolValue : {coolValue = (part1.substring(part1.indexOf(F("_")) + 1, part1.length())).toInt();      break;}
        
      webSockWait=millis();
      webSockUpdate=true;
     
      }

    }
    #if debug
      {
        printMyValues();
      }
#endif
  }
  else  if (com.substring(0, com.indexOf("|")).equalsIgnoreCase("toMega"))
  {
    for (int i = com.indexOf("|"); i < com.lastIndexOf("|"); i = com.indexOf("|", i + 1))
    { String part1 = com.substring(i + 1, com.indexOf("|", i + 1));
      int cmdID = findCommand(part1.substring(0, part1.indexOf(F("_"))).c_str());
      switch (cmdID)
      { //case tPhWert : {sendCommand(part1.substring(0, part1.indexOf(F("_"))), String(PhWert));break;}
        //case tTemp : {sendCommand(part1.substring(0, part1.indexOf(F("_"))), String(Temp));break;}
        case tcalculatedPWM : {sendCommand(part1.substring(0, part1.indexOf(F("_"))), String(calculatedPWM));break;}
        case tcalculatedRed : {sendCommand(part1.substring(0, part1.indexOf(F("_"))), String(calculatedRed));break;}
        case tcalculatedGreen : {sendCommand(part1.substring(0, part1.indexOf(F("_"))), String(calculatedGreen));break;}
        case tcalculatedBlue : {sendCommand(part1.substring(0, part1.indexOf(F("_"))), String(calculatedBlue));break;}
        case tTVModeState : {sendCommand(part1.substring(0, part1.indexOf(F("_"))), String(TVModeState));break;}
        case tcleaningInProcess : {sendCommand(part1.substring(0, part1.indexOf(F("_"))), String(cleaningInProcess));break;}
        case tmanualOverride : {sendCommand(part1.substring(0, part1.indexOf(F("_"))), String(manualOverride));break;}
        case tMoonModeState : {sendCommand(part1.substring(0, part1.indexOf(F("_"))), String(MoonModeState));break;}
        case tpump1Value : {sendCommand(part1.substring(0, part1.indexOf(F("_"))), String(pump1Value));break;}
        case tpump2Value : {sendCommand(part1.substring(0, part1.indexOf(F("_"))), String(pump2Value));break;}
        case tlight230Value : {sendCommand(part1.substring(0, part1.indexOf(F("_"))), String(light230Value));break;}
        case tlight1Value : {sendCommand(part1.substring(0, part1.indexOf(F("_"))), String(light1Value));break;}
        case tlight2Value : {sendCommand(part1.substring(0, part1.indexOf(F("_"))), String(light2Value));break;}
        case tco2Value : {sendCommand(part1.substring(0, part1.indexOf(F("_"))), String(co2Value));break;}
        case theaterValue : {sendCommand(part1.substring(0, part1.indexOf(F("_"))), String(heaterValue));break;}
        case tdPump1Value : {sendCommand(part1.substring(0, part1.indexOf(F("_"))), String(dPump1Value));break;}
        case tdPump2Value : {sendCommand(part1.substring(0, part1.indexOf(F("_"))), String(dPump2Value));break;}
        case tdPump3Value : {sendCommand(part1.substring(0, part1.indexOf(F("_"))), String(dPump3Value));break;}
        case tcoolValue : {sendCommand(part1.substring(0, part1.indexOf(F("_"))), String(coolValue));break;}
       // case tnow : {sendCommand(part1.substring(0, part1.indexOf(F("_"))), String(now.unixtime())); break;}

      }
    }

  }

    else if (com.substring(0, com.indexOf("|")).equalsIgnoreCase("updateMe"))
    {parseCommand(F("toMega|tV|cI|mO|mM|cP|cR|cG|cB|p1|p2|lV|l1|l2|cO|hV|cV|nO"));
          webSocketUpdate();    
    }
    else
    {
      #if debug
      { Serial.println(F("Recieved: "));
        Serial.println(com);
      }
      #endif
     }
  }
  else
  {
    #if debug
      { Serial.println(F("Checksum Wrong Recieved: "));
        Serial.println(com);
      }
      #endif
     }
}




int findCommand(const char* searchText)
{
  int startCount = 0;
  int foundIndex = -1; // -1 = not found
  while (startCount < charCount)
  {
    if (strcmp_P(searchText, (const char*)pgm_read_dword(Char_table + startCount)) == 0)
    {
      foundIndex = startCount;
      break;
    }
    startCount++;
  }
  return foundIndex;
}



