#ifndef LOG_H
#define LOG_H

#include <SD.h>
#include <SPI.h>
#include "gps.h"  // Inclure gps.h pour obtenir les données GPS
#include "globals.h"

// Déclaration des variables pour la carte SD
bool sdAvailable = false;      // Variable pour suivre l'état actuel
bool fileCreated = false;  // Variable pour vérifier si le fichier a déjà été créé
File logFile;
File gpxFile;

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
    if (!SD.exists("/gpx")) {
      SD.mkdir("/gpx");
    }
  }
}

// Fonction pour créer ou ouvrir les fichiers de log (CSV et GPX)
void createLogFiles() {
  if (sdAvailable) {
    // Création d'un fichier CSV et GPX par jour
    char dateBuffer[20];
    sprintf(dateBuffer, "%04d-%02d-%02d", gps.date.year(), gps.date.month(), gps.date.day());

    // Formater le nom des fichiers CSV et GPX avec la date
    char csvFilename[50];
    char gpxFilename[50];
    sprintf(csvFilename, "/csv/%s.csv", dateBuffer);
    sprintf(gpxFilename, "/gpx/%s.gpx", dateBuffer);

    // Ouvrir ou créer le fichier CSV en mode ajout
    logFile = SD.open(csvFilename, FILE_APPEND);
    if (!logFile) {
      logFile = SD.open(csvFilename, FILE_WRITE);
      logFile.println("Date,Heure,Latitude,Longitude,Vitesse(km/h),Altitude(m),Température(°C),Satellites");
      Serial.println("Fichier CSV créé.");
    }

    // Ouvrir ou créer le fichier GPX en mode ajout
    gpxFile = SD.open(gpxFilename, FILE_APPEND);
    if (!gpxFile) {
      gpxFile = SD.open(gpxFilename, FILE_WRITE);
      gpxFile.println("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
      gpxFile.println("<gpx version=\"1.1\" creator=\"GPS Logger\" xmlns=\"http://www.topografix.com/GPX/1/1\">");
      gpxFile.println("<trk><name>GPS Log</name><trkseg>");
      Serial.println("Fichier GPX créé avec l'en-tête.");
    }

    fileCreated = true;  // Indiquer que les fichiers ont été créés
  }
}

// Fonction pour journaliser les données GPS dans les fichiers CSV et GPX
void logGPSData() {
  if (!fileCreated && gps.date.isValid()) {
    createLogFiles();
  }

  if (sdAvailable && fileCreated && currentSatellites >= 4) {
    // Écrire les données dans le fichier CSV
    logFile.printf("%s,%s,%.6f,%.6f,%.2f,%.2f,%.2f,%d\n", 
                   currentDate.c_str(), currentTime.c_str(),
                   currentLatitude, currentLongitude, gps.speed.kmph(), 
                   currentAltitude, currentTemperature, currentSatellites);
    logFile.flush();  // Assurez-vous que les données sont bien écrites

    // Écrire les données dans le fichier GPX
    gpxFile.printf("<trkpt lat=\"%.6f\" lon=\"%.6f\">\n", currentLatitude, currentLongitude);
    gpxFile.printf("<ele>%.2f</ele>\n", currentAltitude);
    gpxFile.printf("<time>%sT%sZ</time>\n", currentDate.c_str(), currentTime.c_str());  // Format ISO 8601
    gpxFile.println("</trkpt>");
    gpxFile.flush();
  }
}

// Fonction pour fermer les fichiers de log et ajouter les balises de fin au fichier GPX
void closeLogFiles() {
  if (logFile) {
    logFile.close();
  }
  if (gpxFile) {
    // Ajouter les balises de fermeture avant de fermer le fichier GPX
    gpxFile.println("</trkseg></trk></gpx>");
    gpxFile.close();
    Serial.println("Fermeture du fichier GPX avec les balises de fin ajoutées.");
  }
}

// Fonction pour traiter et journaliser les données GPS
void processAndLogGPSData() {
  updateGPSData();  // Mettre à jour les données GPS depuis gps.h

  // Journaliser les données GPS dans les fichiers CSV et GPX
  logGPSData();
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
