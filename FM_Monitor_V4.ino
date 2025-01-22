/*********************************
   Program For FM Monitor
   Option: Getdata form blood pressure monitor via UART
   MCU    : ESP32
   Write By : Panya Hantula
   Date: 1 Jul 2024
   Code version: V3.1.3
   Note:
   - Renew Code
   - Update Blynk 2.0 (Edgent)
   - Update Radio Station infomation via Blynk
   - Update Table FW/REF
   - change Library : PZEM, Blynk, LCD 20x2
   - This version is runing without Line Notify

 **********************************/
#define debug true

//---------- Function Prototype ------------------
void CountRunTime(void);
float GetAudioLevel(void);
float ConvertADC_to_dB(float);
void WelcomeNotify(void);
void Power_TX_ON(void);
void Power_TX_OFF(void);
void GetParamALL(void);
void Check_Condition_PWR(void);
void Check_Condition_VSWR(void);
void Check_Condition_Temp(void);
void Check_Condition_Audio(void);
void LCD_ShowInfo(void);
void LCD_ShowParam(void);

//----- Include Header File -----
#include "_Blynk.h"
#include "_DHT.h"
#include "_Power.h"

//----- PIN Config --------------
#define Relay_AC 4
#define Ralay_Excitor 19
#define LED 15
#define ADC_Auido 34

// sda= GPIO_21 /scl= GPIO_22
#define LCD_SDA 21
#define LCD_SCL 22

// https://github.com/locple/LCDI2C_Multilingual
#include <Wire.h>
#include <LCDI2C_Multilingual.h>
LCDI2C_Symbols lcd(0x27, 20, 4);

//----------------------------------------------------------
// #include <esp_task_wdt.h>
// #define WDT_TIMEOUT 60  // WDT Timeout in seconds

//----------------------------------------------------------
// Define NTP Client to get time
#include "time.h"
const char *ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 3.5 * 3600;
const int daylightOffset_sec = 3.5 * 3600;

struct tm timeinfo;
int prev_Min;
int SystemStartTime;

bool SendWelcomeNotify = false;

//---- Multi Task Millis time --------
unsigned long BlynkUpdatePeriod = 1500, Blynk_LastTime = 0;

//================================
//        Setup Function
//================================
void setup() {

  Serial.begin(115200);
  Serial.println("###################################");
  Serial.println("       FM Monitoring System       ");
  Serial.print("     firmware_Version: ");
  Serial.println(BLYNK_FIRMWARE_VERSION);

  //----------------------------------------------------------
  Serial.println(":####  initialize SYSTEM  ####:\n");
  Serial.println(":# initialize OUTPUT Port");
  Serial.print("Relay_AC Port: ");
  Serial.println(Relay_AC);
  Serial.print("Ralay_Excitor Port: ");
  Serial.println(Ralay_Excitor);
  Serial.print("BUZZER Port: ");
  Serial.println(BUZZER);
  Serial.print("LED Port: ");
  Serial.println(LED);

  // Set pinMode
  pinMode(Relay_AC, OUTPUT);
  pinMode(Ralay_Excitor, OUTPUT);
  pinMode(LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  // set default status
  Serial.println("Setup Frist Port: Relay_AC OFF , Ralay_Excitor OFF");
  digitalWrite(Relay_AC, HIGH);
  digitalWrite(Ralay_Excitor, LOW);
  digitalWrite(LED, HIGH);
  beep();
  Serial.println("-> Done\n");

  //----------------------------------------------------------
  Serial.println(":# initialize LCD 20x4");
  Wire.begin(LCD_SDA, LCD_SCL);
  I2C_Scanner();
  lcd.init();
  lcd.backlight();
  LCD_ShowInfo();
  Serial.println("-> Done\n");

  //----------------------------------------------------------
  Serial.println(":# initialize BlynkEdgent");
  BlynkEdgent.begin();
  BlynkEdgent.run();
  Serial.println("-> Done\n");

  //----------------------------------------------------------
  Serial.println(":# initialize Temperature Sensor (DHT11)");
  DHT_Inital();
  Serial.println("-> Done\n");

  //----------------------------------------------------------
  Serial.println(":# initialise EEPROM");
  EEPROM_Inital();
  Serial.println("-> Done\n");

  //----------------------------------------------------------
  Serial.println(":# initialize System Config");
  Get_Config_papameter();
  Serial.println("-> Done\n");

  //----------------------------------------------------------
  Serial.println(":# Initialize a NTPClient");
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  Serial.print("Get Location Time:");
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return;
  } else {
    Serial.println(&timeinfo, "%A, %B %d %Y, %H:%M");

    Serial.println("Get Transamiter Start Time");
    char buffStartTime[40];
    strftime(buffStartTime, 40, "%B %d %Y, %H:%M", &timeinfo);
    for (int i = 0; i < sizeof(buffStartTime); i++)
      StartTime += buffStartTime[i];
  }
  // Get LastMin
  char timeMin[3];
  strftime(timeMin, 3, "%M", &timeinfo);
  prev_Min = atoi(timeMin);
  Serial.println("-> Done");
  //----------------------------------------------------------
  // Serial.println(":# initialize Watchdog");
  // esp_task_wdt_init(WDT_TIMEOUT, true);  // Initialize ESP32 Task WDT
  // esp_task_wdt_add(NULL);                // Subscribe
  // Serial.println("Watchdog Started !!");
  // Serial.println("-> Done\n");

  //-----------------------------------------------------------
  Serial.println(":initialize system -> Done");
  Serial.println();
  Serial.println("#######################################");
  Serial.println("   FM Monitoring System Started !!!   ");
  Serial.println("#######################################");
  Serial.println();

  //----------  Check Protection ------------------
  Serial.println(":# Check AutoPowerOFF config :");
  if (Protection) {
    lcd.setCursor(0, 3);
    lcd.print("AutoPowerOFF -> ON");
    Serial.println("AutoPowerOFF -> ON");
  } else {
    lcd.setCursor(0, 3);
    lcd.print("AutoPowerOFF -> OFF");
    Serial.println("AutoPowerOFF -> OFF");
  }
  delay(2000);
  lcd.clear();

  // ----------- Check last Transamiter Status ---------
  Serial.println(":# Check last Status of FM-Transamiter");
  if (Power_ON_Status) {
    Power_ON_Status = false;
    Power_TX_ON();
    Serial.println(":# POWER ON TX ==> OK !!");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("  POWER TX => ON ");
  } else {
    Serial.println(":# TX Last Status : OFF");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("  POWER TX => OFF ");
  }
  delay(2000);
  lcd.clear();
  Serial.println("-> Done\n");

}
// ##########################################################################
//--------------------------------
//          Main Function
//--------------------------------
void loop() {
  // esp_task_wdt_reset();  // Reset watchdog
  BlynkEdgent.run();

  //-------- Function Call --------
  CountRunTime();
  GetParamALL();
  LCD_ShowParam();
  Check_Condition_PWR();
  Check_Condition_VSWR();
  Check_Condition_Temp();
  Check_Condition_Audio();
  WelcomeNotify();

  //------------ Update Blynk interval-----------------------
  if (millis() - Blynk_LastTime > BlynkUpdatePeriod) {
    Blynk_LastTime = millis();
    BlynkUpload();
  }
}

//----------------------------------------------------------
void CountRunTime(void) {
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return;
  } else {
    TimeNow = "";
    char buffStartTime[40];
    strftime(buffStartTime, 40, "%A, %B %d %Y, %H:%M", &timeinfo);
    for (int i = 0; i < sizeof(buffStartTime); i++)
      TimeNow += buffStartTime[i];

    char timeMin[3];
    strftime(timeMin, 3, "%M", &timeinfo);
    int MinNow = atoi(timeMin);
    Serial.print("Time: ");
    Serial.println(TimeNow);

    if (prev_Min != MinNow) {
      prev_Min = MinNow;
      // Serial.println("Increse 1 min Timer");
      Run_Time_Min++;          // count Minute
      if (Run_Time_Min >= 60)  // check minute to hour
      {
        Run_Time_Hour++;   // increase Hour
        Run_Time_Min = 0;  // Reset Minute
      }
      if (Run_Time_Hour >= 24)  // check minute to hour
      {
        Run_Time_day++;
        Run_Time_Hour = 0;
      }
      if (Run_Time_Min % 5 == 0)  // Update Minute to EEPROM Every 10 minute
      {                           // Upadte Min
        Serial.println("-> Saveing Run_Time_Min");
        EEPROM_Write(231, 234, String(Run_Time_Min));

        Serial.println("-> Saveing Run_Time_Hour");
        EEPROM_Write(235, 238, String(Run_Time_Hour));

        Serial.println("-> Saveing Run_Time_day");
        EEPROM_Write(239, 242, String(Run_Time_day));
      }
    }
  }
  return;
}
//----------------------------------------------------------
void GetParamALL(void) {
  if (debug)
    Serial.println(":# GET Parameter:");

  FW_VALUE = GetFW();
  REF_VALUE = GetREF();
  VSWR = GetVSRW(FW_VALUE, REF_VALUE);
  AudioLevel = GetAudioLevel();
  AudioVU_dB = ConvertADC_to_dB(AudioLevel);

  TX_Temp = GET_TX_Temperature();
  Room_Temp = GET_ROOM_Temperature();

  voltage = GetVoltage();
  current = Getcurrent();
  power = Getpower();
  energy = Getenergy();
  frequency = Getfrequency();
  pf = Getpf();

  if (debug) {
    // Read Present RUN Time
    Serial.print(":# Transmiter Run Time: ");
    Serial.print(Run_Time_day);
    Serial.print(" Day ");
    Serial.print(Run_Time_Hour);
    Serial.print(" Hour ");
    Serial.print(Run_Time_Min);
    Serial.println(" minute");
    Serial.print(":# Wifi Signal: ");
    WifiSignal = String(WiFi.RSSI());
    Serial.println(WifiSignal);

    Serial.println("------------------------------------------------------------");
    Serial.println();
  }
}
//----------------------------------------------------------
float GetAudioLevel(void) {
  float Buff_ADC_Audio = analogRead(ADC_Auido) / 4.0;
  if (debug) {
    Serial.print(":# Reading Audio UV Value... => ");

    Serial.print("Audio ADC: ");
    Serial.print(Buff_ADC_Audio);
  }
  return Buff_ADC_Audio;
}
//----------------------------------------------------------
float ConvertADC_to_dB(float ADC) {
  float dB = (ADC - 280.3754386) / 29.85350877;
  if (debug) {
    Serial.print(" -> ");
    Serial.print(dB);
    Serial.println(" dB");
  }
  return dB;
}
//----------------------------------------------------------
void LCD_ShowInfo(void) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("   FM MONITORING  ");
  lcd.setCursor(0, 1);
  lcd.print("       SYSTEM     ");
  lcd.setCursor(0, 2);
  lcd.print("Version: ");
  lcd.setCursor(9, 2);
  lcd.print(BLYNK_FIRMWARE_VERSION);

  return;
}
//----------------------------------------------------------
void LCD_ShowParam(void) {
  Buff_Srt = "FW:";
  Buff_Srt += int(FW_VALUE);  //
  Buff_Srt += "W REF:";
  Buff_Srt += int(REF_VALUE);  //
  Buff_Srt += "W    ";
  lcd.setCursor(0, 0);
  lcd.print(Buff_Srt);

  Buff_Srt = "TX:";
  Buff_Srt += TX_Temp;
  Buff_Srt += "c";
  Buff_Srt += " VSWR:";
  Buff_Srt += VSWR;
  Buff_Srt += "   ";

  lcd.setCursor(0, 1);
  lcd.print(Buff_Srt);

  Buff_Srt = "Room:";
  Buff_Srt += Room_Temp;
  Buff_Srt += "c     ";
  lcd.setCursor(0, 2);
  lcd.print(Buff_Srt);

  Buff_Srt = "AUDIO:";
  if (AudioLevel > 0.001) {
    Buff_Srt += String(AudioVU_dB);
    Buff_Srt += " dB";
  } else {
    Buff_Srt += "NO AUDIO";
  }
  Buff_Srt += "  ";
  lcd.setCursor(0, 3);
  lcd.print(Buff_Srt);
}
//----------------------------------------------------------
void I2C_Scanner(void) {
  Serial.println();
  Serial.println("I2C scanner. Scanning ...");
  byte count = 0;
  Wire.begin();
  for (byte i = 8; i < 120; i++) {
    Wire.beginTransmission(i);        // Begin I2C transmission Address (i)
    if (Wire.endTransmission() == 0)  // 0 = success(ACK response)
    {
      Serial.print("Found address: ");
      Serial.print(i, DEC);
      Serial.print(" (0x");
      Serial.print(i, HEX);
      Serial.println(")");
      count++;
    }
  }
  Serial.print("Found ");
  Serial.print(count, DEC);  // numbers of devices
  Serial.println(" device(s).");
}
//----------------------------------------------------------
void Check_Condition_PWR(void) {
  //----------- Check FW Critical ------------------
  if (debug) Serial.println("-> Check FW Protection");
  if ((FW_VALUE > FW_Critical_threshold) && Protection) {
    Count_FW_Critical++;

    if ((Count_FW_Critical >= 3))  // กำลังส่งส่งเกินจำกัด
    {
      digitalWrite(Relay_AC, HIGH);
      digitalWrite(Ralay_Excitor, LOW);
      beep_PWR();

      Serial.println("   FW Power Critical ");
      Serial.println(":# TX Power is OFF !!");
      /*
            // Radio Station detial
            Buff_Srt = "\n";
            Buff_Srt += StringRadioStation;
            Buff_Srt += RadioStationName;
            Buff_Srt += "\n";
            Buff_Srt += StringFreq;
            Buff_Srt += RadioFreq;
            Buff_Srt += " MHz \n";
            Buff_Srt += "*****************************";
            Buff_Srt += "\n";
            Buff_Srt += FW_Critical_MSG;
            Buff_Srt += "\n";
            Buff_Srt += "กำลังส่ง : ";  // กำลังส่ง
            Buff_Srt += FW_VALUE;
            Buff_Srt += " วัตต์";  // วัตต์
      */

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("   Power FW is Higher   ");
      lcd.setCursor(0, 1);
      lcd.print("  TRANSMITTER OFF  ");

      Power_ON_Status = false;

      while (true) {
        // esp_task_wdt_reset();  // Reset watchdog
        BlynkEdgent.run();
        GetParamALL();

        //------------ Update Blynk interval-----------------------
        if (millis() - Blynk_LastTime > BlynkUpdatePeriod) {
          Blynk_LastTime = millis();
          BlynkUpload();
        }
      }
    }
  } else {
    Count_FW_Critical = 0;
    // ----------- check FW warning ----------------
    if (debug) Serial.println("-> check FW warning");
    if ((FW_VALUE > FW_warning_threshold) && Notify_Warning) {
      Reset_FW_Warning_Time = millis();  // Get Time for Reset warning

      if (FW_Warning)  // เตือนกำลังส่งสูงเกิน
      {
        Serial.println(" FW Power Higher -> Warning");

        /*
        // Radio Station detial
        Buff_Srt = "\n";
        Buff_Srt += StringRadioStation;
        Buff_Srt += RadioStationName;
        Buff_Srt += "\n";
        Buff_Srt += StringFreq;
        Buff_Srt += RadioFreq;
        Buff_Srt += "\n";
        Buff_Srt += "*****************************";
        Buff_Srt += "\n";
        Buff_Srt += FW_Warning_MSG;
        Buff_Srt += "\n";
        Buff_Srt += "กำลังส่ง : ";  // กำลังส่ง
        Buff_Srt += FW_VALUE;
        Buff_Srt += " วัตต์";  // วัตต์
        sendLine(Buff_Srt);
        */

        FW_Warning = false;
      }
    } else {
      if ((millis() - Reset_FW_Warning_Time > Time_for_Reset_Warning_Check) && (FW_VALUE < (FW_warning_threshold - 20))) {
        FW_Warning = true;
      }
    }
  }
}
//----------------------------------------------------------
void Check_Condition_VSWR(void) {
  //----------- Check VSWR Critical ------------------
  if (debug) Serial.println("-> Check VSWR Protection");
  if ((VSWR > VSWR_Critical_threshold) && Protection) {
    Count_VSWR_Critical++;
    if (Count_VSWR_Critical >= 1) {

      Serial.println("=> FW VSWR Critical");
      Serial.println(":# TX Power is OFF !!");
      digitalWrite(Relay_AC, HIGH);
      digitalWrite(Ralay_Excitor, LOW);

      beep_PWR();

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("   VSWR is Higher   ");
      lcd.setCursor(0, 1);
      lcd.print("  TRANSMITTER OFF  ");

      /*
        // Radio Station detial
        Buff_Srt = "\n";
        Buff_Srt += StringRadioStation;
        Buff_Srt += RadioStationName;
        Buff_Srt += "\n";
        Buff_Srt += StringFreq;
        Buff_Srt += RadioFreq;
        Buff_Srt += " MHz \n";
        Buff_Srt += "*****************************";
        Buff_Srt += "\n";
        Buff_Srt += VSWR_Critical_MSG;
        Buff_Srt += "\n";
        Buff_Srt += "VSWR : ";
        Buff_Srt += VSWR;
        Buff_Srt += "\n";
        Buff_Srt += "กำลังส่ง : ";  // กำลังส่ง
        Buff_Srt += FW_VALUE;
        Buff_Srt += " วัตต์";  // วัตต์
        Buff_Srt += "\n";
        Buff_Srt += "กำลังสะท้อน : ";
        Buff_Srt += REF_VALUE;
        Buff_Srt += " วัตต์";  // วัตต์

        sendLine(Buff_Srt);
        */

      Power_ON_Status = false;

      while (true) {
        // esp_task_wdt_reset();  // Reset watchdog
        BlynkEdgent.run();
        GetParamALL();
        //------------ Update Blynk interval-----------------------
        if (millis() - Blynk_LastTime > BlynkUpdatePeriod) {
          Blynk_LastTime = millis();
          BlynkUpload();
        }
      }
    }
  } else {
    Count_VSWR_Critical = 0;
    // ----------- check VSWR warning ----------------
    if (debug) Serial.println("-> check VSWR warning");
    if ((VSWR > VSWR_warning_threshold) && Notify_Warning) {
      Reset_VSWR_Warning_Time = millis();  // Get Time for Reset warning
      if (VSWR_Warning) {                  // Radio Station detial
        Serial.println("=> FW VSWR Warning");

        /*
        Buff_Srt = "\n";
        Buff_Srt += StringRadioStation;
        Buff_Srt += RadioStationName;
        Buff_Srt += "\n";
        Buff_Srt += StringFreq;
        Buff_Srt += RadioFreq;
        Buff_Srt += "\n";
        Buff_Srt += "*****************************";
        Buff_Srt += "\n";
        Buff_Srt += VSWR_Warning_MSG;
        Buff_Srt += "\n";
        Buff_Srt += "VSWR : ";
        Buff_Srt += VSWR;
        Buff_Srt += "\n";
        Buff_Srt += "กำลังส่ง : ";  // กำลังส่ง
        Buff_Srt += FW_VALUE;
        Buff_Srt += " วัตต์";  // วัตต์
        Buff_Srt += "\n";
        Buff_Srt += "กำลังสะท้อน : ";
        Buff_Srt += REF_VALUE;
        Buff_Srt += " วัตต์";  // วัตต์

        sendLine(Buff_Srt);
        */

        VSWR_Warning = false;
      }
    } else {
      if ((millis() - Reset_VSWR_Warning_Time > Time_for_Reset_Warning_Check) && (VSWR < (VSWR_warning_threshold - 0.12))) {
        VSWR_Warning = true;
      }
    }
  }
}
//----------------------------------------------------------
void Check_Condition_Temp(void) {
  // ----------- check TX_Temp warning ----------------
  if (debug) Serial.println("-> check TX_Temp warning");
  if ((TX_Temp > TX_Temp_Warning_threshold) && Notify_Warning) {
    Reset_TX_Temp_Warning_Time = millis();  // Get Time for Reset warning
    if (TX_Temp_Warning) {
      Serial.println("=> TX Temperature Warning");

      /*
      // Radio Station detial
        Buff_Srt = "\n";
        Buff_Srt += StringRadioStation;
        Buff_Srt += RadioStationName;
        Buff_Srt += "\n";
        Buff_Srt += StringFreq;
        Buff_Srt += RadioFreq;
        Buff_Srt += " MHz \n";
        Buff_Srt += "*****************************";
        Buff_Srt += "\n";
        Buff_Srt += TX_Temp_Warning_MSG;
        Buff_Srt += "\n";
        Buff_Srt += "อุณหภูมิ : ";
        Buff_Srt += TX_Temp;
        Buff_Srt += " องศา";

        sendLine(Buff_Srt);
        */

      TX_Temp_Warning = false;
    }
  } else {
    if ((millis() - Reset_TX_Temp_Warning_Time > Time_for_Reset_Warning_Check) && (TX_Temp < (TX_Temp_Warning_threshold - 2))) {
      TX_Temp_Warning = true;
    }
  }

  // ----------- check Room_Temp warning ----------------
  if (debug) Serial.println("-> check Room_Temp warning");
  if ((Room_Temp > ROOM_Temp_Warning_threshold) && Notify_Warning) {
    ROOM_Temp_Warning_Time = millis();  // Get Time for Reset warning
    if (ROOM_Temp_Warning) {

      Serial.println("=> TX Temperature Warning");

      /*
      // Radio Station detial
        Buff_Srt = "\n";
        Buff_Srt += StringRadioStation;
        Buff_Srt += RadioStationName;
        Buff_Srt += "\n";
        Buff_Srt += StringFreq;
        Buff_Srt += RadioFreq;
        Buff_Srt += " MHz \n";
        Buff_Srt += "*****************************";
        Buff_Srt += "\n";
        Buff_Srt += ROOM_Temp_Warning_MSG;
        Buff_Srt += "\n";
        Buff_Srt += "อุณหภูมิ : ";
        Buff_Srt += Room_Temp;
        Buff_Srt += " องศา";

        sendLine(Buff_Srt);
        */
      ROOM_Temp_Warning = false;
    }
  } else {
    if ((millis() - ROOM_Temp_Warning_Time > Time_for_Reset_Warning_Check) && (Room_Temp < (ROOM_Temp_Warning_threshold - 2))) {
      ROOM_Temp_Warning = true;
    }
  }
}
//----------------------------------------------------------
void Check_Condition_Audio(void) {
  // ----------- check Audio warning ----------------
  if (debug) Serial.println("-> check Audio warning");
  if ((AudioLevel < 10) && Power_ON_Status && Notify_Warning) {
    Audio_Warning_Count++;

    if ((Audio_Warning_Count > 120) && (Audio_Warning_Count < 300) && Audio_Warning_First) {

      Serial.println("=> Audio Loss 2 minute Warning");

      /*
      // Radio Station detial
        Buff_Srt = "\n";
        Buff_Srt += StringRadioStation;
        Buff_Srt += RadioStationName;
        Buff_Srt += "\n";
        Buff_Srt += StringFreq;
        Buff_Srt += RadioFreq;
        Buff_Srt += " MHz \n";

        Buff_Srt += "*****************************";
        Buff_Srt += "\n";
        Buff_Srt += Audio_Warning_First_MSG;

        sendLine(Buff_Srt);
        */
      Audio_Warning_First = false;
    } else if ((Audio_Warning_Count > 300) && Audio_Warning_Seconed) {
      Serial.println("=> Audio Loss 5 minute Warning");

      /*
      // Radio Station detial
        Buff_Srt = "\n";
        Buff_Srt += StringRadioStation;
        Buff_Srt += RadioStationName;
        Buff_Srt += "\n";
        Buff_Srt += StringFreq;
        Buff_Srt += RadioFreq;
        Buff_Srt += " MHz \n";

        Buff_Srt += "*****************************";
        Buff_Srt += "\n";
        Buff_Srt += Audio_Warning_Seconed_MSG;

        sendLine(Buff_Srt);
        */
      Audio_Warning_Seconed = false;
    } else if (!Audio_Warning_First && !Audio_Warning_Seconed) {
      Audio_Warning_Count = 0;
    }
  } else {
    Audio_Warning_First = true;
    Audio_Warning_Seconed = true;
    Audio_Warning_Count = 0;
  }
}
//----------------------------------------------------------
void Power_TX_ON(void) {
  Serial.println(":# Turn ON Power Amplifier Precess !!");
  Serial.println("   Turn On Exciter and wait 20 Sec befor Start PA");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("  POWER TX -> ON ");
  if (!Power_ON_Status) {
    lcd.setCursor(0, 2);
    lcd.print("                    ");
    lcd.setCursor(0, 2);
    lcd.print("EXCITER STARTED");
    beep_PWR();

    // Start Excitor First
    digitalWrite(Ralay_Excitor, HIGH);

    // wait 20 Sec befor Start PA
    for (int i = 0; i < 20; i++) {
      // esp_task_wdt_reset();  // Reset watchdog

      lcd.setCursor(i, 3);
      lcd.print("#");
      delay(700);
      Serial.print(i);
      Serial.print(" ");
    }

    Serial.println();
    lcd.setCursor(0, 2);
    lcd.print("                    ");
    lcd.setCursor(0, 2);
    lcd.print("PA STARTED");

    beep_PWR();

    // Trun ON PA after 20 sec
    digitalWrite(Relay_AC, LOW);

    Power_ON_Status = true;
    EEPROM_Write(160, 162, "1");
    String Get = EEPROM_Read(160, 162);
    Serial.print("-> Power_ON_Status : ");
    Serial.println(Get);

    Serial.println(":# Send Notify : Power ON ");
    Serial.println(":# TX Power is ON !!");

    /*
    // Radio Station detial
    Buff_Srt = "\n";
    Buff_Srt += Power_On_MSG;
    Buff_Srt += "\n";
    Buff_Srt += "*****************************";
    Buff_Srt += "\n";
    Buff_Srt += StringRadioStation;
    Buff_Srt += RadioStationName;
    Buff_Srt += "\n";
    Buff_Srt += StringFreq;
    Buff_Srt += RadioFreq;
    Buff_Srt += " MHz \n";
    Buff_Srt += StringStationID;
    Buff_Srt += RadioStationID;

    sendLine(Buff_Srt);
*/
    Serial.println("GET Parameter with about 60 secend");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("-> POWER TX -> ON ");
    lcd.setCursor(0, 1);
    lcd.print("   Waiting 60 Sec   ");
    lcd.setCursor(0, 2);
    lcd.print("Get TX Parameter !! ");

    for (int i = 0; i < 20; i++) {
      // esp_task_wdt_reset();  // Reset watchdog
      lcd.setCursor(i, 3);
      lcd.print("*");
      delay(200);
      Serial.print(i);
      Serial.print(" ");
    }
    Serial.println();
    delay(2000);
    lcd.clear();

    StartTime = "";
    Serial.print("Get Location Time:");
    if (!getLocalTime(&timeinfo)) {
      Serial.println("Failed to obtain time");
      return;
    } else {
      Serial.println(&timeinfo, "%A, %B %d %Y, %H:%M");

      Serial.println("Get Transamiter Start Time");
      char buffStartTime[40];
      strftime(buffStartTime, 40, "%B %d %Y, %H:%M", &timeinfo);
      for (int i = 0; i < sizeof(buffStartTime); i++)
        StartTime += buffStartTime[i];
    }
    // Get Start system start time
    SystemStartTime = Run_Time_Min;
    SendWelcomeNotify = true;
  }
  return;
}
//----------------------------------------------------------
void Power_TX_OFF(void) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("  POWER TX => ON ");
  if (Power_ON_Status) {
    beep_PWR();

    Serial.println(":# TX Power is OFF !!");
    digitalWrite(Relay_AC, HIGH);
    digitalWrite(Ralay_Excitor, LOW);

    lcd.setCursor(0, 0);
    lcd.print("-> POWER TX => OFF !! ");
    Serial.println(":# Send Notify : Power Off  ");
    /*
        // Radio Station detial
        Buff_Srt = "\n";
        Buff_Srt += Power_Off_MSG;
        Buff_Srt += "\n";
        Buff_Srt += "*****************************";
        Buff_Srt += "\n";
        Buff_Srt += StringRadioStation;
        Buff_Srt += RadioStationName;
        Buff_Srt += " \n";
        Buff_Srt += StringFreq;
        Buff_Srt += RadioFreq;
        Buff_Srt += " MHz \n";
        Buff_Srt += StringStationID;
        Buff_Srt += RadioStationID;

        sendLine(Buff_Srt);
    */

    Power_ON_Status = false;
    EEPROM_Write(160, 162, "0");
    String Get = EEPROM_Read(160, 162);
    Serial.print("-> Power_ON_Status : ");
    Serial.println(Get);
    delay(5000);
    lcd.clear();
  }
  return;
}
//----------------------------------------------------------
void WelcomeNotify(void) {
  if ((Run_Time_Min >= (SystemStartTime + 2)) && SendWelcomeNotify) {
    SendWelcomeNotify = false;
    beep();
    /*
  Buff_Srt = "\n";
  Buff_Srt += StringRadioStation;
  Buff_Srt += RadioStationName;
  Buff_Srt += "\n";
  Buff_Srt += StringFreq;
  Buff_Srt += RadioFreq;
  Buff_Srt += " MHz \n";
  Buff_Srt += "*****************************";
  Buff_Srt += "\n";
  Buff_Srt += "รายงานสถานะเครื่องส่งวิทยุ";
  Buff_Srt += "\n";
  Buff_Srt += "กำลังส่ง : ";  // กำลังส่ง
  Buff_Srt += FW_VALUE;
  Buff_Srt += " วัตต์";  // วัตต์
  Buff_Srt += "\n";
  Buff_Srt += "กำลังสะท้อน : ";
  Buff_Srt += REF_VALUE;
  Buff_Srt += " วัตต์";  // วัตต์
  Buff_Srt += "\n";
  Buff_Srt += "VSWR : ";
  Buff_Srt += VSWR;
  Buff_Srt += "\n";
  Buff_Srt += "อุณหภูมิเครื่องส่ง : ";
  Buff_Srt += TX_Temp;
  Buff_Srt += " องศา";
  Buff_Srt += "\n";
  Buff_Srt += "อุณหภูมิห้อง : ";
  Buff_Srt += Room_Temp;
  Buff_Srt += " องศา";

  Buff_Srt += "\n";
  Buff_Srt += "\n";
  Buff_Srt += "เปิดใช้งานเครื่องส่งวิทยุมาแล้ว ";
  Buff_Srt += "\n";
  Buff_Srt += String(Run_Time_day);
  Buff_Srt += " วัน ";
  Buff_Srt += String(Run_Time_Hour);
  Buff_Srt += " ชั่วโมง ";
  Buff_Srt += String(Run_Time_Min);
  Buff_Srt += " นาที";
  Buff_Srt += "\n";

  Serial.println(":# Send Line_Notify -> First Parameter  ");
  sendLine(Buff_Srt);
  */
  }
}