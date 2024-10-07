#ifndef LOG_H
#define LOG_H

#include <SD.h>
#include <SPI.h>
#include "gps.h"  // Inclure gps.h pour obtenir les données GPS
#include "globals.h"

// Déclaration des variables pour la carte SD
bool sdAvailable = false;      // Variable pour suivre l'état actuel
bool fileCreated = false;      // Variable pour vérifier si le fichier a déjà été créé

// Fonction pour initialiser la carte SD avec une fréquence réduite à 4 MHz
void initSDCard() {
  SPI.begin(SPI_SCLK, SPI_MISO, SPI_MOSI, SD_CS);  // Initialisation du bus SPI

  if (!SD.begin(SD_CS, SPI, 4000000)) {
    Serial.println("Erreur : carte SD non détectée !");
    while (true);  // Boucle infinie si la carte SD n'est pas détectée
  } else {
    Serial.println("Carte SD initialisée avec succès.");
    sdAvailable = true;

    // Création des dossiers si nécessaire
    if (!SD.exists("/csv")) {
      SD.mkdir("/csv");
    }
  }
}

// Fonction pour créer ou ouvrir le fichier CSV après validation des satellites et de la date
void createCSVFile() {
  if (sdAvailable && gps.date.isValid() && currentSatellites >= 4) {
    Serial.println("Création du fichier de log CSV...");

    // Formater le nom du fichier CSV avec la date actuelle
    char csvFilename[50];
    sprintf(csvFilename, "/csv/%04d-%02d-%02d.csv", gps.date.year(), gps.date.month(), gps.date.day());

    // Ouvrir ou créer le fichier CSV en mode ajout
    File logFile = SD.open(csvFilename, FILE_APPEND);
    if (!logFile) {
      logFile = SD.open(csvFilename, FILE_WRITE);
      logFile.println("Date,Heure,Latitude,Longitude,Vitesse(km/h),Altitude(m),Température(°C),Satellites");
      Serial.println("Fichier CSV créé.");
    }
    logFile.close();

    fileCreated = true;  // Indiquer que le fichier a été créé
  } else if (currentSatellites < 4) {
    Serial.println("Pas assez de satellites, en attente de données GPS valides pour créer le fichier.");
  }
}

// Fonction pour journaliser les données GPS dans le fichier CSV
void logGPSDataToCSV() {
  updateGPSData();
  updateTemperature(); 
  if (!fileCreated && gps.date.isValid() && currentSatellites >= 4) {
    createCSVFile();
  }

  if (sdAvailable && fileCreated) {
    // Calcul du décalage horaire
    int adjustedHour = gps.time.hour() + timezoneOffset;
    if (adjustedHour >= 24) {
      adjustedHour -= 24;
    } else if (adjustedHour < 0) {
      adjustedHour += 24;
    }

    // Formater l'heure ajustée
    char timeBuffer[9];
    sprintf(timeBuffer, "%02d:%02d:%02d", adjustedHour, gps.time.minute(), gps.time.second());

    // Formater le nom du fichier CSV avec la date actuelle
    char csvFilename[50];
    sprintf(csvFilename, "/csv/%04d-%02d-%02d.csv", gps.date.year(), gps.date.month(), gps.date.day());

    // Ouvrir, écrire et refermer le fichier CSV pour chaque log
    File logFile = SD.open(csvFilename, FILE_APPEND);
    if (logFile) {
      logFile.printf("%s,%s,%.6f,%.6f,%.2f,%.2f,%.2d,%d\n", 
                     currentDate.c_str(), timeBuffer,
                     currentLatitude, currentLongitude, gps.speed.kmph(), 
                     currentAltitude, currentTemperature,currentSatellites);
      logFile.close();
      Serial.println("Données GPS journalisées.");
    } else {
      Serial.println("Erreur : Impossible d'ouvrir le fichier CSV pour l'écriture.");
    }
  }
}

// Fonction pour traiter et journaliser les données GPS dans le fichier CSV
void processAndLogGPSData() {
  updateGPSData();  // Mettre à jour les données GPS depuis gps.h

  // Journaliser les données GPS dans le fichier CSV
  logGPSDataToCSV();
}
bool restartSD() {
  bool sdAvailable = SD.exists("/");

  // Si la carte SD n'est plus disponible, tenter de réinitialiser la carte SD
  if (sdAvailable == false) {
    Serial.println("Carte SD non disponible, tentative de réinitialisation...");
    
    // Forcer la désactivation de la carte SD
    SD.end();
    delay(500);  // Attendre un peu avant de réinitialiser
    
    // Réinitialiser la carte SD
    if (SD.begin(SD_CS, SPI, 4000000)) {
      sdAvailable = SD.exists("/");  // Mise à jour après tentative d'initialisation
      if (sdAvailable) {
        Serial.println("Carte SD réinitialisée avec succès.");
        ESP.restart();  // Redémarrer l'ESP
      }
    } else {
      Serial.println("Échec de l'initialisation de la carte SD après réinsertion.");
      sdAvailable = false;
    }
  }

  return sdAvailable;
}

#endif
