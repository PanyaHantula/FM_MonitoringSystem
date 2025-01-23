//--------------------------------------------------------------
#include <PZEM004Tv30.h>

#define PZEM_SERIAL Serial1
#define PZEM_RX_PIN 16
#define PZEM_TX_PIN 17
PZEM004Tv30 pzem(PZEM_SERIAL, PZEM_RX_PIN, PZEM_TX_PIN);

//------ Function Prototype ---------------
float GetVoltage(void);
float Getcurrent(void);
float Getpower(void);
float Getenergy(void);
float Getfrequency(void);
float Getpf(void);

//--------------------------------------------------------------
float GetVoltage(void) {
  float voltage = pzem.voltage();
  if (!isnan(voltage)) {
    if (debug) {
      Serial.print("\t- Voltage: ");
      Serial.print(voltage);
      Serial.println("V");
    }
  } else {
    Serial.println("- Error reading voltage");
    voltage = 0;
  }
  return voltage;
}
//--------------------------------------------------------------
float Getcurrent(void) {
  float current = pzem.current();
  if (!isnan(current)) {
    if (debug) {
      Serial.print("\t- Current: ");
      Serial.print(current);
      Serial.println("A");
    }
  } else {
    Serial.println("\t- Error reading current");
    current = 0;
  }
  return current;
}
//--------------------------------------------------------------
float Getpower(void) {
  float power = pzem.power();
  if (!isnan(power)) {
    if (debug) {
      Serial.print("\t- Power: ");
      Serial.print(power);
      Serial.println("W");
    }
  } else {
    Serial.println("\t- Error reading power");
    power = 0;
  }
  return power;
}
//--------------------------------------------------------------
float Getenergy(void) {
  float energy = pzem.energy();
  if (!isnan(energy)) {
    if (debug) {
      Serial.print("\t- Energy: ");
      Serial.print(energy, 3);
      Serial.println("kWh");
    }
  } else {
    Serial.println("\t- Error reading energy");
    energy = 0;
  }
  return energy;
}
//--------------------------------------------------------------
float Getfrequency(void) {
  float frequency = pzem.frequency();
  if (!isnan(frequency)) {
    if (debug) {
      Serial.print("\t- Frequency: ");
      Serial.print(frequency, 1);
      Serial.println("Hz");
    }
  } else {
    Serial.println("\t- Error reading frequency");
    frequency = 0;
  }
  return frequency;
}
//--------------------------------------------------------------
float Getpf(void) {
  float pf = pzem.pf();
  if (!isnan(pf)) {
    if (debug) {
      Serial.print("\t- PF: ");
      Serial.println(pf);
    }
  } else {
    Serial.println("\t- Error reading power factor");
    pf = 0;
  }
  return pf;
}
