
#define ADC_FW 32
#define ADC_REF 35

// for Power Amp 500 Watt Version
float Power_Watt[] = { 0, 0.5, 5, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120, 130, 140, 150, 160, 170, 180, 190, 200, 210, 220, 230, 240, 250, 260, 270, 280, 290, 300, 310, 320, 330, 340, 350, 360, 370, 380, 390, 400, 410, 420, 430, 440, 450, 460, 470, 480, 490, 500, 510, 520, 530, 540, 550, 560, 570, 580, 590, 600 };
float Foward_v[] = { 0.00, 0.01, 0.68, 0.16, 0.35, 0.50, 0.65, 0.75, 0.76, 0.81, 0.93, 1.00, 1.10, 1.16, 1.24, 1.32, 1.40, 1.46, 1.53, 1.61, 1.67, 1.74, 1.80, 1.87, 1.94, 2.00, 2.06, 2.14, 2.19, 2.25, 2.31, 2.37, 2.43, 2.47, 2.52, 2.56, 2.60, 2.65, 2.70, 2.75, 2.79, 2.84, 2.87, 2.92, 2.96, 3.00, 3.04, 3.07, 3.11, 3.15, 3.19, 3.20, 3.26, 3.28, 3.31, 3.33, 3.36, 3.40, 3.45, 3.50, 3.55, 3.60, 3.65 };
float Reflec_Watt[] = { 0.5, 1, 3, 4, 5, 10, 15, 20, 25, 30, 35, 40 };
float Reflec_v[] = { 0.00, 0.01, 0.03, 0.04, 0.05, 0.15, 0.25, 0.34, 0.44, 0.51, 0.59, 0.65 };

// for Power Amp 1500 Watt Version
//float Power_Watt[] = {0, 10, 50, 100, 150, 200, 250, 300, 350, 400, 450, 500, 550, 600, 650, 700, 750, 800, 850, 900, 1000, 1050, 1100, 1150, 1200, 1250, 1300, 1350, 1400, 1450, 1500};
//float Foward_v[] = {0.00, 0.10, 0.40, 0.60, 0.78, 0.90, 1.00, 1.10, 1.20, 1.30, 1.38, 1.48, 1.56, 1.64, 1.70, 1.78, 1.85, 1.90, 1.98, 2.00, 2.16, 2.22, 2.30, 2.35, 2.40, 2.45, 2.50, 2.55, 2.60, 2.65, 2.70};
//float Reflec_Watt[] = {0.5, 1, 3, 4, 5, 10, 15, 20, 25, 30, 35, 40};
//float Reflec_v[] = {0.00, 0.01, 0.03, 0.04, 0.05, 0.15, 0.25, 0.34, 0.44, 0.51, 0.59, 0.65};

//--------- Function Prototype --------------------
float GetFW(void);
float GetREF(void);
float GetVSRW(float, float);

//--------------------------------------------------------------
float GetFW(void) {
  if (debug) Serial.print(":# Reading FW Value.. => ");

  int index = 0, i;
  int ALL_index = 31;  // for Power Amp 1500 Watt Version
  // int ALL_index = 63; // for Power Amp 500 Watt Version

  float Vin_FW = 0, FW_VALUE;
  for (i = 0; i < 1000; i++)  // Average ADC Value
  {
    int sensorValue1 = analogRead(ADC_FW);
    Vin_FW = Vin_FW + (float(sensorValue1) / 4095.0 * 3.3);
  }

  Vin_FW = float(Vin_FW / 1000);
  if (Vin_FW > 0.001) {
    Vin_FW += 0.2;  // volteOffset_factor;

    for (i = 1; i < ALL_index; i++) {
      if ((Foward_v[i - 1] < Vin_FW) && (Foward_v[i] > Vin_FW)) {
        index = i;
        i = ALL_index + 1;
      }
    }
    float Y1 = Power_Watt[index - 1];
    float Y2 = Power_Watt[index];
    float X1 = Foward_v[index - 1];
    float X2 = Foward_v[index];

    FW_VALUE = ((Y2 - Y1) * ((Vin_FW - X1) / (X2 - X1))) + Y1;
  } else {
    FW_VALUE = 0;
    if (debug) {
      Serial.print(" FW :");
      Serial.print(FW_VALUE);
      Serial.println();
    }
  }
  return FW_VALUE;
}
//--------------------------------------------------------------
float GetREF(void) {
  if (debug) Serial.print(":# Reading REF Value... => ");

  int index_Ref = 0, i_Ref, ALL_index_Ref = 12;  // for NBTC Version 20
  float Vin_REF = 0, REF_VALUE;
  for (i_Ref = 0; i_Ref < 1000; i_Ref++) {
    int sensorValue2 = analogRead(ADC_REF);
    Vin_REF = Vin_REF + (float(sensorValue2) / 4095.0 * 3.3);
  }

  Vin_REF = float(Vin_REF / 1000);
  //-------- V ref offset --------
  if (Vin_REF > 0.001) {
    Vin_REF += 0;  // volteOffset_factor;

    //---------
    for (i_Ref = 1; i_Ref < ALL_index_Ref; i_Ref++) {
      if ((Reflec_v[i_Ref - 1] < Vin_REF) && (Reflec_v[i_Ref] > Vin_REF)) {
        index_Ref = i_Ref;
        i_Ref = ALL_index_Ref + 1;
      }
    }
    float Y3 = Reflec_Watt[index_Ref - 1];
    float Y4 = Reflec_Watt[index_Ref];
    float X3 = Reflec_v[index_Ref - 1];
    float X4 = Reflec_v[index_Ref];

    REF_VALUE = ((Y4 - Y3) * ((Vin_REF - X3) / (X4 - X3))) + Y3;
  } else {
    REF_VALUE = 0;
    if (debug) {
      Serial.print(" REF:");
      Serial.print(REF_VALUE);
      Serial.println();
    }
  }

  return REF_VALUE;
}
//--------------------------------------------------------------
float GetVSRW(float FW, float REF) {
  float VSWR;

  if (debug) Serial.print(":# VSRW Value... => ");
  if (FW > 0) {
    float A = sqrt(REF / FW);
    VSWR = (1 + sqrt(REF / FW)) / (1 - sqrt(REF / FW));
  } else {
    VSWR = 0;
  }
  if (debug) {
    Serial.print(" VSWR: ");
    Serial.println(VSWR);
  }

  return VSWR;
}
