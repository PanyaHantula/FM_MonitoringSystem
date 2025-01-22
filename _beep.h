#define BUZZER 2

//----- Function Prototype ---------------
void beep(void);
void beep_PWR(void);

//--------------------------------------------------------------
void beep(void) {
  digitalWrite(BUZZER, LOW);
  delay(200);
  digitalWrite(BUZZER, HIGH);
  delay(200);
}
//--------------------------------------------------------------
void beep_PWR(void) {
  digitalWrite(BUZZER, LOW);
  delay(100);
  digitalWrite(BUZZER, HIGH);
  delay(100);
  digitalWrite(BUZZER, LOW);
  delay(1000);
  digitalWrite(BUZZER, HIGH);
}