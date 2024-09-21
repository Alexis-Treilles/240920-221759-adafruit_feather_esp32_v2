#ifndef GPS_UPDATE_H
#define GPS_UPDATE_H

#include <TinyGPS++.h>
#include "globals.h"

// Déclarer l'objet GPS
extern TinyGPSPlus gps;

// Déclarer la variable externe pour le décalage horaire (par défaut à +2)
int timezoneOffset = 2;  // Par défaut, décalage de 2 heures

// Fonction pour mettre à jour les informations GPS
void updateGPSData() {
  // Met à jour le nombre de satellites si valide
  if (gps.satellites.isValid()) {
    currentSatellites = gps.satellites.value();
  }
  
  // Met à jour la latitude et la longitude si valides
  if (gps.location.isValid()) {
    currentLatitude = gps.location.lat();
    currentLongitude = gps.location.lng();
  }

  // Met à jour l'heure si elle est valide
  if (gps.time.isValid()) {
    int hour = gps.time.hour() + timezoneOffset;  // Ajoute le décalage horaire

    // Ajuster l'heure si elle dépasse 24h ou est inférieure à 0
    if (hour >= 24) {
      hour -= 24;
    } else if (hour < 0) {
      hour += 24;
    }

    char timeBuffer[6]; // Stockage temporaire pour l'heure formatée
    sprintf(timeBuffer, "%02d:%02d", hour, gps.time.minute());
    currentTime = String(timeBuffer);  // Met à jour l'heure globale au format "HH:MM"
  }
}

#endif
