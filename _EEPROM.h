#include "_ParamConfig.h"
#include "_PZEM.h"

void Get_Config_papameter(void);
void EEPROM_Write(int, int, String);
String EEPROM_Read(int, int);

//---------------------------------------------------------------------
void EEPROM_Inital(void);

//************ EEPROM Setup *************
#include "EEPROM.h"
int EEPROM_SIZE = 256, addr = 0;

//---------------------------------------------------------------------
void EEPROM_Inital(void) {
  if (!EEPROM.begin(EEPROM_SIZE)) {
    Serial.println("failed to initialise EEPROM");
    delay(1000000);
  }
  Serial.println(":# EEPROM OK! ");
  return;
}

//---------------------------------------------------------------------
void Get_Config_papameter(void) {

  FW_warning_threshold = EEPROM_Read(110, 116).toFloat();
  Serial.print("-> FW_warning_threshold : ");
  Serial.println(FW_warning_threshold);

  VSWR_warning_threshold = EEPROM_Read(117, 123).toFloat();
  Serial.print("-> VSWR_warning_threshold : ");
  Serial.println(VSWR_warning_threshold);

  TX_Temp_Warning_threshold = EEPROM_Read(124, 130).toFloat();
  Serial.print("-> TX_Temp_Warning_threshold : ");
  Serial.println(TX_Temp_Warning_threshold);

  ROOM_Temp_Warning_threshold = EEPROM_Read(131, 138).toFloat();
  Serial.print("-> ROOM_Temp_Warning_threshold : ");
  Serial.println(ROOM_Temp_Warning_threshold);

  FW_Critical_threshold = EEPROM_Read(139, 146).toFloat();
  Serial.print("-> FW_Critical_threshold : ");
  Serial.println(FW_Critical_threshold);

  VSWR_Critical_threshold = EEPROM_Read(147, 154).toFloat();
  Serial.print("-> VSWR_Critical_threshold : ");
  Serial.println(VSWR_Critical_threshold);

  String Get = EEPROM_Read(152, 154);
  Notify_Audio = Get == "1";
  Serial.print("-> Notify_Audio : ");
  Serial.println(Notify_Audio);

  Get = EEPROM_Read(154, 156);
  Notify_Warning = Get == "1";
  Serial.print("-> Notify_Warning : ");
  Serial.println(Notify_Warning);

  Get = EEPROM_Read(156, 158);
  Protection = Get == "1";
  Serial.print("-> Protection : ");
  Serial.println(Protection);

  Get = EEPROM_Read(158, 160);
  Notify_Line = Get == "1";
  Serial.print("-> Notify_Line : ");
  Serial.println(Notify_Line);

  Get = EEPROM_Read(160, 162);
  Power_ON_Status = Get == "1";
  Serial.print("-> Power_ON_Status : ");
  Serial.println(Power_ON_Status);

  RadioStationName = EEPROM_Read(163, 203);
  Serial.print("-> Radio Station Name : ");
  Serial.println(RadioStationName);

  RadioFreq = EEPROM_Read(204, 211);
  Serial.print("-> Freqyency : ");
  Serial.println(RadioFreq);

  RadioStationID = EEPROM_Read(212, 221);
  Serial.print("-> RadioStationID : ");
  Serial.println(RadioStationID);

  Run_Time_Min = EEPROM_Read(231, 234).toInt();
  Serial.print("-> Run_Time_Min : ");
  Serial.println(Run_Time_Min);

  Run_Time_Hour = EEPROM_Read(235, 238).toInt();
  Serial.print("-> Run_Time_Hour : ");
  Serial.println(Run_Time_Hour);

  Run_Time_day = EEPROM_Read(239, 242).toInt();
  Serial.print("-> Run_Time_day : ");
  Serial.println(Run_Time_day);
}

//---------------------------------------------------------------------
String EEPROM_Read(int Start_address, int End_address) {
  int r;
  String ret = "";

  for (int i = 0; i < End_address - Start_address; ++i) {
    r = EEPROM.read(Start_address + i);
    if (r == 0)
      break;
    else
      ret += char(r);
  }
  // Serial.println("Write EEPROM Sucess !!");
  // Serial.print("Data form EEPROM: ");
  // Serial.println(ret);

  return ret;
}
//---------------------------------------------------------------------
void EEPROM_Write(int Start_address, int End_address, String p_text) {
  char text[End_address - Start_address];

  p_text.toCharArray(text, (End_address - Start_address));
  for (int i = 0; i < End_address - Start_address; ++i)
    EEPROM.write(Start_address + i, 0);
  for (int i = 0; i < p_text.length(); ++i)
    EEPROM.write(Start_address + i, text[i]);

  EEPROM.commit();
  Serial.println("Write EEPROM Sucess !!");
}