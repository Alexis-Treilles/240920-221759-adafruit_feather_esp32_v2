#ifndef LOG_H
#define LOG_H

#include <TinyGPS++.h>
#include "SD.h"

// Variables pour stocker la position précédente et la distance parcourue
float previousLatitude = 0.0;
float previousLongitude = 0.0;
float distanceTraveled = 0.0;

bool sdAvailable = false;  // Variable pour vérifier si la SD est initialisée

// Fonction pour initialiser la carte SD
void initSDCard() {
  if (!SD.begin()) {
    Serial.println("Erreur : Carte SD non disponible !");
    sdAvailable = false;
  } else {
    Serial.println("Carte SD initialisée avec succès.");
    sdAvailable = true;
  }
}

// Fonction pour initialiser le fichier de log (ex : fichier par jour)
void initLogFile(int day, int month, int year) {
  if (sdAvailable) {  // Vérifier si la carte SD est disponible
    // Formater le nom du fichier
    char filename[20];
    sprintf(filename, "%04d-%02d-%02d.csv", year, month, day);

    // Créer ou ouvrir le fichier sur la carte SD
    File logFile = SD.open(filename, FILE_WRITE);
    if (logFile) {
      // Écrire l'entête du fichier CSV
      logFile.println("Date,Heure,Latitude,Longitude,Vitesse(km/h),Distance(m)");
      logFile.close();
    } else {
      Serial.println("Erreur d'ouverture du fichier CSV.");
    }
  } else {
    Serial.println("Carte SD non initialisée, fichier de log non créé.");
  }
}

// Fonction pour enregistrer les données GPS dans un fichier CSV
void logDataToCSV(const char* date, const char* time, float latitude, float longitude, float speed, float distance) {
  if (sdAvailable) {  // Vérifier si la carte SD est disponible
    // Formater le nom du fichier
    char filename[20];
    sprintf(filename, "%s.csv", date);

    // Ouvrir le fichier en mode ajout
    File logFile = SD.open(filename, FILE_WRITE);
    if (logFile) {
      // Formater et écrire les données dans le fichier CSV
      logFile.printf("%s,%s,%.6f,%.6f,%.2f,%.2f\n", date, time, latitude, longitude, speed, distance);
      logFile.close();
    } else {
      Serial.println("Erreur d'ouverture du fichier CSV pour journalisation.");
    }
  } else {
    Serial.println("Carte SD non disponible, journalisation échouée.");
  }
}

// Fonction pour enregistrer les données GPS au format GPX
void logDataToGPX(float latitude, float longitude, float altitude, const char* time) {
  if (sdAvailable) {  // Vérifier si la carte SD est disponible
    File gpxFile = SD.open("track.gpx", FILE_WRITE);
    if (gpxFile) {
      gpxFile.printf("<trkpt lat=\"%.6f\" lon=\"%.6f\">\n", latitude, longitude);
      gpxFile.printf("<ele>%.2f</ele>\n", altitude);
      gpxFile.printf("<time>%s</time>\n", time);
      gpxFile.println("</trkpt>");
      gpxFile.close();
    } else {
      Serial.println("Erreur d'ouverture du fichier GPX pour journalisation.");
    }
  } else {
    Serial.println("Carte SD non disponible, journalisation GPX échouée.");
  }
}

// Fonction pour traiter et journaliser les données GPS
void processAndLogGPSData(TinyGPSPlus& gps, HardwareSerial& SerialGPS) {
  // Lire les données GPS
  while (SerialGPS.available() > 0) {
    gps.encode(SerialGPS.read());
  }

  // Si les données GPS sont valides, journaliser les informations
  if (gps.location.isValid()) {
    // Calculer la distance parcourue depuis la dernière position connue
    if (previousLatitude != 0.0 && previousLongitude != 0.0) {
      distanceTraveled += gps.distanceBetween(previousLatitude, previousLongitude, gps.location.lat(), gps.location.lng());
    }

    // Mettre à jour les coordonnées précédentes
    previousLatitude = gps.location.lat();
    previousLongitude = gps.location.lng();

    // Formater les données pour la journalisation
    char dateBuffer[11];  // Stocker la date formatée
    char timeBuffer[9];   // Stocker l'heure formatée
    sprintf(dateBuffer, "%04d-%02d-%02d", gps.date.year(), gps.date.month(), gps.date.day());
    sprintf(timeBuffer, "%02d:%02d:%02d", gps.time.hour(), gps.time.minute(), gps.time.second());

    // Journaliser dans les fichiers CSV et GPX
    logDataToCSV(dateBuffer, timeBuffer, gps.location.lat(), gps.location.lng(), gps.speed.kmph(), distanceTraveled);
    logDataToGPX(gps.location.lat(), gps.location.lng(), gps.altitude.meters(), timeBuffer);
  }
}

#endif
