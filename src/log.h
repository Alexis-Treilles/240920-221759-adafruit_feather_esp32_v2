#ifndef LOG_H
#define LOG_H

#include <SD.h>
#include <SPI.h>
#include "gps.h"  // Inclure gps.h pour obtenir les données GPS
#include "globals.h"

// Déclaration des variables pour la carte SD
bool sdAvailable = false;  // Variable pour vérifier si la carte SD est initialisée
bool fileCreated = false;  // Variable pour vérifier si le fichier a déjà été créé
File logFile;
File gpxFile;

// Déclaration des broches SPI
#define SD_CS 5
#define SPI_SCLK 18
#define SPI_MISO 19
#define SPI_MOSI 23

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

// Fonction pour ouvrir ou créer un fichier de log (CSV et GPX) avec la date comme nom
void createLogFile() {
  if (sdAvailable) {
    Serial.println("Création  des fichiers de log...");
    // Formater le nom du fichier avec la date actuelle pour le CSV (utilisation de tirets au lieu de slash)
    char csvFilename[40];
    sprintf(csvFilename, "/csv/%04d-%02d-%02d.csv", gps.date.year(), gps.date.month(), gps.date.day());

    // Ouvrir ou créer le fichier CSV en mode ajout
    logFile = SD.open(csvFilename, FILE_APPEND);
    if (!logFile) {
      logFile = SD.open(csvFilename, FILE_WRITE);
      logFile.println("Date,Heure,Latitude,Longitude,Vitesse(km/h),Altitude(m)");
      Serial.println("Fichier CSV créé.");
    }

    // Formater le nom du fichier avec la date actuelle pour le GPX (utilisation de tirets au lieu de slash)
    char gpxFilename[40];
    sprintf(gpxFilename, "/gpx/%04d-%02d-%02d.gpx", gps.date.year(), gps.date.month(), gps.date.day());

    // Ouvrir ou créer le fichier GPX en mode ajout
    gpxFile = SD.open(gpxFilename, FILE_APPEND);
    if (!gpxFile) {
      gpxFile = SD.open(gpxFilename, FILE_WRITE);
      gpxFile.println("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
      gpxFile.println("<gpx version=\"1.1\" creator=\"GPS Logger\" xmlns=\"http://www.topografix.com/GPX/1/1\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:schemaLocation=\"http://www.topografix.com/GPX/1/1 http://www.topografix.com/GPX/1/1/gpx.xsd\">");
      gpxFile.println("<trk><name>GPS Log</name><trkseg>");
      Serial.println("Fichier GPX créé.");
    }

    fileCreated = true;  // Indique que les fichiers ont été créés
  }
}

// Fonction pour journaliser les données GPS dans les fichiers CSV et GPX
void logGPSData() {
  // Ne créer les fichiers de log que si la date est valide et si ce n'est pas encore fait
  if (!fileCreated && currentDate != "2000-00-00") {
    createLogFile();
  }

  if (sdAvailable && fileCreated) {
    // Écrire les données dans le fichier CSV
    logFile.printf("%s,%s,%.6f,%.6f,%.2f,%.2f\n", currentDate.c_str(), currentTime.c_str(),
                   currentLatitude, currentLongitude, gps.speed.kmph(), currentAltitude);
    logFile.flush();  // Assurez-vous que les données sont bien écrites

    // Écrire les données dans le fichier GPX
    gpxFile.printf("<trkpt lat=\"%.6f\" lon=\"%.6f\">\n", currentLatitude, currentLongitude);
    gpxFile.printf("<ele>%.2f</ele>\n", currentAltitude);
    gpxFile.printf("<time>%sT%sZ</time>\n", currentDate.c_str(), currentTime.c_str());
    gpxFile.println("</trkpt>");
    gpxFile.flush();  // Assurez-vous que les données sont bien écrites
  } else if (currentDate == "2000-00-00") {
    Serial.println("Date invalide, en attente de données GPS valides pour créer le fichier.");
  }
}

// Fonction pour fermer les fichiers de log
void closeLogFiles() {
  if (logFile) {
    logFile.close();
  }
  if (gpxFile) {
    gpxFile.println("</trkseg></trk></gpx>");
    gpxFile.close();
  }
}

// Fonction pour traiter et journaliser les données GPS
void processAndLogGPSData() {
  updateGPSData();  // Appel de la fonction qui met à jour les données GPS depuis gps.h

  // Journaliser les données GPS dans les fichiers CSV et GPX
  logGPSData();
}

#endif
