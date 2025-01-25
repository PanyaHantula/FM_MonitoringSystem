#define ADC_Auido 34

double dBu[] = { -48, -24, -12, -4, 0, 4, 8, 14, 20 };
float audio_adc[] = { 0, 50, 100, 190, 470, 870, 1600, 3800, 4000 };

//----------------------------------------------------------
float GetAudioLevel(void) {
  Serial.println(":# Reading Audio UV Value...");

  int index = 0, i;
  int LengthOfIndexAudio = 9;

  float Vin_audio = 0, audio_dBu;
  for (i = 0; i < 10; i++)  // Average ADC Value
  {
    int sensorValue1 = analogRead(ADC_Auido);
    Vin_audio = Vin_audio + (float(sensorValue1));
  }

  Vin_audio = float(Vin_audio / 10);
  Serial.print("Vin_audio :" + String(Vin_audio));

  if (Vin_audio >= 4000) {
    audio_dBu = 20;
  } else {
    // Searching Index of Vin_FW
    for (i = 1; i < LengthOfIndexAudio; i++) {  //Serial.println("audio_adc[i - 1] : " + String(audio_adc[i - 1]) + "Vin_audio : " + String(Vin_audio) + "audio_adc[i] : " + String(audio_adc[i]));
      if ((audio_adc[i - 1] < Vin_audio) && (audio_adc[i] > Vin_audio)) {
        index = i;
        i = LengthOfIndexAudio + 1;
      }
    }
    //Serial.println("Index : " + String(index));

    double Y1 = dBu[index - 1];
    double Y2 = dBu[index];
    float X1 = audio_adc[index - 1];
    float X2 = audio_adc[index];

    // Calculate Interpolate FW value
    audio_dBu = ((Y2 - Y1) * ((Vin_audio - X1) / (X2 - X1))) + Y1;
/*
    Serial.print("Index: ");
    Serial.print(index);
    Serial.print(" ,Y1: ");
    Serial.print(Y1);
    Serial.print(",Y2:");
    Serial.print(Y2);
    Serial.print(" ,X1:");
    Serial.print(X1);
    Serial.print(",X2:");
    Serial.print(X2);
    Serial.print(" ,Vin_audio:");
    Serial.print(Vin_audio);
    Serial.print(" ,audio_dBu:");
    Serial.print(audio_dBu);
    Serial.println();
    */
  }

  Serial.print(" -> Audio (dBu) :");
  Serial.print(audio_dBu);
  Serial.println();

  return audio_dBu;
}
//----------------------------------------------------------
float ConvertADC_to_dB(float ADC) {

  /*
    float dB = (ADC - 280.3754386) / 29.85350877;
    if (debug) {
      Serial.print(" -> ");
      Serial.print(dB);
      Serial.println(" dB");
    }
    return dB;
    */
}