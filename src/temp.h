#ifndef TEMP_H
#define TEMP_H
#include <OneWire.h>
#include <DallasTemperature.h>
#include "globals.h"
// Pin à laquelle est connecté le DS18B20
#define ONE_WIRE_BUS 14


// Assure-toi que la variable sensors et currentTemperature sont bien définies
extern DallasTemperature sensors;
void updateTemperature() {
  sensors.requestTemperatures(); // Demande la température
  float tempC = sensors.getTempCByIndex(0); // Obtenir la température en Celsius
  if (tempC != DEVICE_DISCONNECTED_C) {
    currentTemperature = (int)tempC; // Mettre à jour la variable globale
  } else {
    Serial.println("Erreur de lecture du capteur DS18B20");
  }
}

#endif
