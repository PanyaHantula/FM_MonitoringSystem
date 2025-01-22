//************ Line Notify Setup *************
#include <TridentTD_LineNotify.h>

String LINE_TOKEN = "6uhy69fjGNptRzSZ03pzbncmH9GCBF2ZnwgNJ42PlH7";
//String LINE_TOKEN  = "82oHIGS32YHGRdjJMCRy3OTiAROMEfZIfGrlkrKApZl";

#define Power_On_MSG "เปิดเครื่องส่งวิทยุ"
#define Power_Off_MSG "ปิดเครื่องส่งวิทยุผ่าน App Blynk"
#define StringRadioStation "สถานีวิทยุ : ";
#define StringFreq "ความถี่ : ";
#define StringStationID "รหัสสถานี : ";

void Connect_Line(void);
void sendLine(String);
//--------------------------------------------------------------
void Connect_Line(void) {

  Serial.print("LINE TOKEN: ");
  Serial.println(LINE_TOKEN);
  Serial.println(LINE.getVersion());

  LINE.setToken(LINE_TOKEN);
  LINE.notify("เปิดระบบตรวจสอบเครื่องส่งวิทยุแล้ว");
  Serial.println("Send Line Notify Frist time --> Done !!");
  return;
}

//--------------------------------------------------------------
void sendLine(String txt){
    LINE.setToken(LINE_TOKEN);
  LINE.notify(txt);
  Serial.println("Send Line Notify Frist time --> Done !!");
    return;
}