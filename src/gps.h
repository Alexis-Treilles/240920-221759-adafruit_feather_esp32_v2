#ifndef GPS_H
#define GPS_H

#include <TinyGPS++.h>
#include <HardwareSerial.h>
#include "globals.h"

// Déclarer l'objet GPS et la liaison série GPS
extern TinyGPSPlus gps;
extern HardwareSerial SerialGPS;
extern int timezoneOffset;  // Déclaration de la variable pour le décalage horaire

// Fonction d'initialisation du GPS
void initGPS() {
  // Initialisation de Serial1 pour la communication GPS (RX -> GPIO16, TX -> GPIO17)
  SerialGPS.begin(9600, SERIAL_8N1, 16, 17);
  Serial.println("Initialisation du GPS Neo-6M...");
  tft.setCursor(60,220);
  tft.println("GPS");
}

// Fonction de mise à jour des données GPS
// Fonction de mise à jour des données GPS sans blocage
void updateGPSData() {
  // Vérifie si des données sont disponibles depuis le GPS
  if (SerialGPS.available() > 0) {
    // Lire les données GPS et les traiter
    gps.encode(SerialGPS.read());

    // Si une nouvelle position est disponible, met à jour les informations GPS
    if (gps.location.isUpdated()) {
      currentLatitude = gps.location.lat();
      currentLongitude = gps.location.lng();
      currentSpeed = gps.speed.kmph();
      currentAltitude = gps.altitude.meters();

      // Affiche dans la console pour débogage
      //Serial.println(currentAltitude);
      //Serial.print("Latitude : ");
      //Serial.println(currentLatitude, 6);
      //Serial.print("Longitude : ");
      //Serial.println(currentLongitude, 6);
      //Serial.print("Vitesse (km/h) : ");
      //Serial.println(currentSpeed);
      //Serial.print("Altitude (m) : ");
    }

    // Mise à jour du nombre de satellites
    if (gps.satellites.isUpdated()) {
      currentSatellites = gps.satellites.value();
      //Serial.print("Satellites : ");
      //Serial.println(currentSatellites);
    }

    // Met à jour la date et l'heure si disponibles
    if (gps.date.isUpdated() && gps.time.isUpdated()) {
      char dateBuffer[11];  // Stockage temporaire pour la date formatée
      sprintf(dateBuffer, "%02d/%02d/%04d", gps.date.day(), gps.date.month(), gps.date.year());
      currentDate = String(dateBuffer);

      // Ajouter le décalage horaire
      int adjustedHour = gps.time.hour() + timezoneOffset;
      if (adjustedHour >= 24) {
        adjustedHour -= 24;
      } else if (adjustedHour < 0) {
        adjustedHour += 24;
      }

      char timeBuffer[9];  // Stockage temporaire pour l'heure formatée
      sprintf(timeBuffer, "%02d:%02d:%02d", adjustedHour, gps.time.minute(), gps.time.second());
      currentTime = String(timeBuffer);

      // Affiche la date et l'heure dans la console pour débogage
      //Serial.print("Date : ");
      //Serial.println(currentDate);
      //Serial.print("Heure (avec décalage) : ");
      //Serial.println(currentTime);
    }
  }
}

#endif
