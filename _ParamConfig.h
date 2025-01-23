
//--------------------------------------------------------------
String Buff_Srt;

float FW_VALUE, REF_VALUE, VSWR;
float Room_Temp, TX_Temp;
float AudioLevel,AudioVU_dB;
float voltage, current, power, energy, frequency, pf;
int Day, Hour, Min;
String WifiSignal;

String StartTime = "",TimeNow = "";
unsigned int Run_Time_day;
unsigned int Run_Time_Hour;
unsigned int Run_Time_Min;

String RadioFreq = "";
String RadioStationID = "";
String RadioStationName = "";

bool Notify_Line;
bool Notify_Audio;
bool Notify_Warning;
bool Protection;

bool FW_Warning = true;
bool VSWR_Warning = true;
bool TX_Temp_Warning = true;
bool ROOM_Temp_Warning = true;
bool voltage_Warning = true;

bool Power_ON_Status;

float FW_warning_threshold;
float VSWR_warning_threshold;
float TX_Temp_Warning_threshold;
float ROOM_Temp_Warning_threshold;
float FW_Critical_threshold;
float VSWR_Critical_threshold;

int Time_for_Reset_Warning_Check  = 60000;
int Reset_FW_Warning_Time;
int Reset_VSWR_Warning_Time;
int Reset_TX_Temp_Warning_Time;
int ROOM_Temp_Warning_Time;
int voltage_Warning_Time;
int Count_FW_Critical = 0;
int Count_VSWR_Critical = 0;

String FW_Warning_MSG = "กำลังส่งสูงผิดปกติ \nโปรดตรวจสอบ";
String VSWR_Warning_MSG = "ค่า VSWR สูงผิดปกติ \nโปรดตรวจสอบ";
String TX_Temp_Warning_MSG = "อุณหภูมิเครื่องส่งสูงผิดปกติ \nโปรดตรวจสอบ";
String ROOM_Temp_Warning_MSG = "อุณหภูมิห้องสูงผิดปกติ \nโปรดตรวจสอบ";
String FW_Critical_MSG = "กำลังส่งสูงเกินขีดจำกัด \nปิดเครื่องส่งวิทยุอัตโนมัติ";
String VSWR_Critical_MSG = "ค่า VSWR สูงเกินขีดจำกัด \nปิดเครื่องส่งวิทยุอัตโนมัติ";

int Audio_Warning_Count = 0;
bool Audio_Warning_First = true;
bool Audio_Warning_Seconed = true;

String Audio_Warning_First_MSG = "สัญญาณเสียงขาดหายมาแล้ว 2 นาที \nโปรดตรวจสอบ";
String Audio_Warning_Seconed_MSG = "สัญญาณเสียงขาดหายมาแล้ว 5 นาที \nโปรดตรวจสอบ";

// float lattitude = 14.923493, longtitude = 102.029515;
float lattitude = 0.0, longtitude = 0.0;
String Location = "-";

#define Power_On_MSG "เปิดเครื่องส่งวิทยุ"
#define Power_Off_MSG "ปิดเครื่องส่งวิทยุผ่าน App Blynk"
#define StringRadioStation "สถานีวิทยุ : ";
#define StringFreq "ความถี่ : ";
#define StringStationID "รหัสสถานี : ";