//************ DHT Temperature Sensor Setup *************
#define DHT_DEBUG

void DHT_Inital(void);
float GET_TX_Temperature(void);
float GET_ROOM_Temperature(void);

//--------------------------------------------------------------
#include "DHT.h"
#define DHTPIN 18
#define DHT2PIN 5
#define DHTTYPE DHT11
#define DHT2TYPE DHT11

DHT DHT_TX(DHTPIN, DHTTYPE);
DHT DHT_ROOM(DHT2PIN, DHT2TYPE);

float Temp;
//--------------------------------------------------------------

void DHT_Inital(void) {
  Serial.println("DHT_Inital DHT11");
  Serial.print("DHT_TX PIN:");
  Serial.print(DHTPIN);
  Serial.print(" , TYPE:");
  Serial.println(DHTTYPE);

  Serial.print("DHT_ROOM PIN:");
  Serial.print(DHT2PIN);
  Serial.print(" , TYPE:");
  Serial.println(DHT2TYPE);

  DHT_ROOM.begin();
  DHT_TX.begin();

  Serial.println("Test Get Temperature ....");
  Serial.println("Humidity (%)\tTemperature (C)\tPosition");
  float humidity = DHT_TX.readHumidity();
  float temperature = DHT_TX.readTemperature();

  Serial.print(humidity, 1);
  Serial.print("\t\t");
  Serial.print(temperature, 1);
  Serial.print("\t\t");
  Serial.println("Tx Temp");

  humidity = DHT_ROOM.readHumidity();
  temperature = DHT_ROOM.readTemperature();

  Serial.print(humidity, 1);
  Serial.print("\t\t");
  Serial.print(temperature, 1);
  Serial.print("\t\t");
  Serial.println("Room Temp");
}
//--------------------------------------------------------------
float GET_TX_Temperature(void) {
  Temp = DHT_TX.readTemperature();
#ifdef DHT_DEBUG
  // check if returns are valid, if they are NaN (not a number) then something went wrong!
  if (isnan(Temp)) {
    Serial.println("Failed to read from DHT");
    Temp = 0;
  } else {
    if (debug) {
      Serial.print(":# TX Temp: ");
      Serial.print(Temp);
      Serial.println(" *C");
    }
  }
#endif
  return Temp;
}
//--------------------------------------------------------------
float GET_ROOM_Temperature(void) {
  Temp = DHT_ROOM.readTemperature();
#ifdef DHT_DEBUG
  if (isnan(Temp)) {
    Serial.println("Failed to read from DHT");
    Temp = 0;
  } else {
    if (debug) {
      Serial.print(":# ROOM Temp: ");
      Serial.print(Temp);
      Serial.println(" *C");
    }
  }
#endif
  return Temp;
}