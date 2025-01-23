
#include "_EEPROM.h"
#include "_beep.h"

//----- Function Prototype ---------------

//--------------------------------------------------------------
#define BLYNK_TEMPLATE_ID "TMPL6Gf2TIhaM"
#define BLYNK_FIRMWARE_VERSION "4.1.1"
#define BLYNK_TEMPLATE_NAME "FMmonitor"

#define BLYNK_PRINT Serial
#define APP_DEBUG
#define USE_ESP32_DEV_MODULE

#include "BlynkEdgent.h"
//--------------------------------------------------------------
void BlynkUpload(void) {
  Blynk.virtualWrite(V1, float(FW_VALUE));
  Blynk.virtualWrite(V2, float(REF_VALUE));
  Blynk.virtualWrite(V3, float(TX_Temp));
  Blynk.virtualWrite(V5, float(Room_Temp));
  Blynk.virtualWrite(V7, float(AudioVU_dB));
  Blynk.virtualWrite(V39, String(AudioVU_dB));
  Blynk.virtualWrite(V13, VSWR);
  Blynk.virtualWrite(V24, voltage);
  Blynk.virtualWrite(V25, current);
  Blynk.virtualWrite(V26, power);
  Blynk.virtualWrite(V27, energy);
  Blynk.virtualWrite(V28, frequency);
  Blynk.virtualWrite(V29, pf);
  Blynk.virtualWrite(V4, String(RadioStationName));
  Blynk.virtualWrite(V31, String(RadioFreq));
  Blynk.virtualWrite(V30, String(RadioStationID));
  Blynk.virtualWrite(V33, Run_Time_day);
  Blynk.virtualWrite(V34, Run_Time_Hour);
  Blynk.virtualWrite(V35, Run_Time_Min);
  Blynk.virtualWrite(V36, String(WiFi.RSSI()));
  Blynk.virtualWrite(V37, StartTime);
  Blynk.virtualWrite(V38, TimeNow);
}

//-------- ON-OFF Power Tx ---------------
BLYNK_WRITE(V8) {
  Serial.print("Get SW Power ON: ");
  String SW_Power_ON = param.asStr();
  Serial.println(SW_Power_ON);

  if (SW_Power_ON == "1") {
    Power_TX_ON();
  } else if (SW_Power_ON == "0") {
    Power_TX_OFF();
  }

  delay(200);
  return;
}

//---------- Set Enable Line Notify ----------------
BLYNK_WRITE(V17) {
  Serial.print("Get Notify_Line: ");
  String Get = param.asStr();
  Serial.println(Get);
  beep();
  if (Get == "1") {
    Notify_Line = true;
    EEPROM_Write(158, 160, "1");
  } else if (Get == "0") {
    Notify_Line = false;
    EEPROM_Write(158, 160, "0");
  }

  Get = EEPROM_Read(158, 160);
  Serial.print("-> Notify_Line : ");
  Serial.println(Get);

  return;
}

//--------Set Enable Audio Notify----------------
BLYNK_WRITE(V14) {
  Serial.print("Get Notify Audio: ");
  String Get = param.asStr();
  Serial.println(Get);
  beep();

  if (Get == "1") {
    Notify_Audio = true;
    EEPROM_Write(152, 154, "1");
  } else if (Get == "0") {
    Notify_Audio = false;
    EEPROM_Write(152, 154, "0");
  }

  Get = EEPROM_Read(152, 154);
  Serial.print("-> Notify_Audio : ");
  Serial.println(Get);

  return;
}

//---------Set Enable Notify Warning--------------
BLYNK_WRITE(V15) {
  Serial.print("Get Notify_Warning: ");
  String Get = param.asStr();
  Serial.println(Get);

  beep();

  if (Get == "1") {
    Notify_Warning = true;
    EEPROM_Write(154, 156, "1");
  } else if (Get == "0") {
    Notify_Warning = false;
    EEPROM_Write(154, 156, "0");
  }

  Get = EEPROM_Read(154, 156);
  Serial.print("-> Notify_Warning : ");
  Serial.println(Get);

  return;
}
//-------- Critical Warning------------
BLYNK_WRITE(V16) {
  Serial.print("Set Auto Critical Power OFF: ");
  String Get = param.asStr();
  Serial.println(Get);
  beep();
  if (Get == "1") {
    Protection = true;
    EEPROM_Write(156, 158, "1");
  } else if (Get == "0") {
    Protection = false;
    EEPROM_Write(156, 158, "0");
  }

  Get = EEPROM_Read(156, 158);
  Serial.print("-> Notify_Critical : ");
  Serial.println(Get);

  return;
}

//--------------------------------------------------------------
BLYNK_WRITE(V18) {
  Serial.print("Get VSWR_warning_threshold: ");
  String Get = param.asStr();
  VSWR_warning_threshold = Get.toFloat();
  Serial.println(VSWR_warning_threshold);
  beep();

  EEPROM_Write(117, 123, String(VSWR_warning_threshold));
  VSWR_warning_threshold = EEPROM_Read(117, 123).toFloat();
  Serial.print("-> VSWR_warning_threshold : ");
  Serial.println(VSWR_warning_threshold);

  return;
}
//--------------------------------------------------------------
BLYNK_WRITE(V19) {
  Serial.print("Get FW_warning_threshold: ");
  String Get = param.asStr();
  FW_warning_threshold = Get.toFloat();
  Serial.println(FW_warning_threshold);
  beep();

  EEPROM_Write(110, 116, String(FW_warning_threshold));
  FW_warning_threshold = EEPROM_Read(110, 116).toFloat();
  Serial.print("-> FW_warning_threshold : ");
  Serial.println(FW_warning_threshold);

  return;
}
//--------------------------------------------------------------
BLYNK_WRITE(V20) {
  Serial.print("Get TX_Temp_Warning_threshold: ");
  String Get = param.asStr();
  TX_Temp_Warning_threshold = Get.toFloat();
  Serial.println(TX_Temp_Warning_threshold);
  beep();

  EEPROM_Write(124, 130, String(TX_Temp_Warning_threshold));
  TX_Temp_Warning_threshold = EEPROM_Read(124, 130).toFloat();
  Serial.print("-> TX_Temp_Warning_threshold : ");
  Serial.println(TX_Temp_Warning_threshold);
  return;
}
//--------------------------------------------------------------
BLYNK_WRITE(V21) {
  Serial.print("Get Room_Temp_Warning_threshold: ");
  String Get = param.asStr();
  ROOM_Temp_Warning_threshold = Get.toFloat();
  Serial.println(ROOM_Temp_Warning_threshold);
  beep();

  EEPROM_Write(131, 138, String(ROOM_Temp_Warning_threshold));
  ROOM_Temp_Warning_threshold = EEPROM_Read(131, 138).toFloat();
  Serial.print("-> ROOM_Temp_Warning_threshold : ");
  Serial.println(ROOM_Temp_Warning_threshold);

  return;
}
//--------------------------------------------------------------
BLYNK_WRITE(V22) {
  Serial.print("Get FW_Critical_threshold: ");
  String Get = param.asStr();
  FW_Critical_threshold = Get.toFloat();
  Serial.println(FW_Critical_threshold);
  beep();

  EEPROM_Write(139, 146, String(FW_Critical_threshold));
  FW_Critical_threshold = EEPROM_Read(139, 146).toFloat();
  Serial.print("-> FW_Critical_threshold : ");
  Serial.println(FW_Critical_threshold);
  return;
}
//--------------------------------------------------------------
BLYNK_WRITE(V23) {
  Serial.print("Get VSWR_Critical_threshold: ");
  String Get = param.asStr();
  VSWR_Critical_threshold = Get.toFloat();
  Serial.println(VSWR_Critical_threshold);
  beep();

  EEPROM_Write(147, 145, String(VSWR_Critical_threshold));
  VSWR_Critical_threshold = EEPROM_Read(147, 154).toFloat();
  Serial.print("-> VSWR_Critical_threshold : ");
  Serial.println(VSWR_Critical_threshold);
  return;
}
//--------------------------------------------------------------
BLYNK_WRITE(V32) {
  Serial.print("Get Reset Energy: ");
  String ResetEnergy = param.asStr();
  Serial.println(ResetEnergy);
  if (ResetEnergy == "1") {
    pzem.resetEnergy();
  }
  beep();
  delay(200);
  return;
}
//--------------------------------------------------------------
BLYNK_WRITE(V10) {
  Serial.print("Get Radio Station Name: ");
  RadioStationName = param.asStr();
  Serial.println(RadioStationName);
  EEPROM_Write(163, 203, RadioStationName);
  beep();

  delay(200);
  return;
}
//--------------------------------------------------------------
BLYNK_WRITE(V9) {
  Serial.print("Get Radio Station ID: ");
  RadioStationID = param.asStr();
  Serial.println(RadioStationID);
  EEPROM_Write(212, 221, RadioStationID);
  beep();
  delay(200);
  return;
}
//--------------------------------------------------------------
BLYNK_WRITE(V11) {
  Serial.print("Get Radio Station Frequency: ");
  RadioFreq = param.asStr();
  Serial.println(RadioFreq);
  EEPROM_Write(204, 211, RadioFreq);
  beep();
  delay(200);
  return;
}
//--------------------------------------------------------------
BLYNK_WRITE(V6) {
  Serial.print("Reset Run Time: ");
  String ResetRunTime = param.asStr();
  Serial.println(ResetRunTime);
  if (ResetRunTime == "1") {

    Serial.print(":# Transmiter Reset Run Time: ");

    Run_Time_day = 0;
    Run_Time_Hour = 0;
    Run_Time_Min = 0;

    Serial.print(Run_Time_day);
    Serial.print(" Day ");
    Serial.print(Run_Time_Hour);
    Serial.print(" Hour ");
    Serial.print(Run_Time_Min);

    Serial.println("-> Saveing Run_Time_Min");
    EEPROM_Write(231, 234, String(Run_Time_Min));

    Serial.println("-> Saveing Run_Time_Hour");
    EEPROM_Write(235, 238, String(Run_Time_Hour));

    Serial.println("-> Saveing Run_Time_day");
    EEPROM_Write(239, 242, String(Run_Time_day));
  }
  beep();
  delay(200);
  return;
}