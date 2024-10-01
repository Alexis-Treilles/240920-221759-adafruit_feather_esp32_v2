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



// Fonction pour initialiser la carte SD avec une fréquence réduite à 4 MHz
void initSDCard() {
  SPI.begin(SPI_SCLK, SPI_MISO, SPI_MOSI, SD_CS);  // Initialisation du bus SPI

  if (!SD.begin(SD_CS, SPI, 4000000)) {
    Serial.println("Erreur : carte SD non détectée !");
    while (true);  // Boucle infinie si la carte SD n'est pas détectée
  } else {
    Serial.println("Carte SD initialisée avec succès.");
    sdAvailable = true;
    tft.setCursor(180,220);
    tft.println("SD");

    // Création des dossiers si nécessaire
    if (!SD.exists("/csv")) {
      SD.mkdir("/csv");
    }
    if (!SD.exists("/gpx")) {
      SD.mkdir("/gpx");
    }
  }
}

// Fonction pour supprimer les 3 dernières lignes du fichier GPX si le fichier existe déjà
void removeLastThreeLines(const char* filename) {
  if (SD.exists(filename)) {
    File gpxFile = SD.open(filename, FILE_READ);

    if (!gpxFile) {
      Serial.println("Erreur : impossible d'ouvrir le fichier GPX !");
      return;
    }

    // Lire le fichier dans un buffer et retirer les 3 dernières lignes
    String fileContent = "";
    while (gpxFile.available()) {
      fileContent += char(gpxFile.read());
    }
    gpxFile.close();

    // Localiser la dernière balise </trkpt> pour supprimer les lignes finales
    int lastClosingTag = fileContent.lastIndexOf("</trkpt>");
    if (lastClosingTag != -1) {
      fileContent = fileContent.substring(0, lastClosingTag) + "</trkpt>\n";
    }

    // Réécrire le fichier avec le contenu modifié
    gpxFile = SD.open(filename, FILE_WRITE);
    gpxFile.print(fileContent);
    gpxFile.close();

    Serial.println("Fichier GPX existant : les 3 dernières lignes ont été supprimées.");
  }
}
// Fonction pour créer ou ouvrir les fichiers de log (CSV et GPX)
void createLogFile() {
  if (sdAvailable) {
    Serial.println("Création des fichiers de log...");

    // Formater le nom du fichier CSV avec la date et l'heure actuelle
    char csvFilename[50];
    sprintf(csvFilename, "/csv/%04d-%02d-%02d_%02d-%02d-%02d.csv", 
            gps.date.year(), gps.date.month(), gps.date.day(), 
            gps.time.hour(), gps.time.minute(), gps.time.second());

    // Ouvrir ou créer le fichier CSV en mode ajout
    logFile = SD.open(csvFilename, FILE_APPEND);
    if (!logFile) {
      logFile = SD.open(csvFilename, FILE_WRITE);
      logFile.println("Date,Heure,Latitude,Longitude,Vitesse(km/h),Altitude(m)");
      Serial.println("Fichier CSV créé.");
    }

    // Formater le nom du fichier GPX avec la date et l'heure actuelle
    char gpxFilename[50];
    sprintf(gpxFilename, "/gpx/%04d-%02d-%02d_%02d-%02d-%02d.gpx", 
            gps.date.year(), gps.date.month(), gps.date.day(), 
            gps.time.hour(), gps.time.minute(), gps.time.second());

    // Vérifier si le fichier existe
    if (SD.exists(gpxFilename)) {
      // Si le fichier existe, ouvrir et supprimer les 3 dernières lignes
      gpxFile = SD.open(gpxFilename, FILE_APPEND);
      if (gpxFile) {
        removeLastThreeLines(gpxFilename);
      } else {
        Serial.println("Erreur : impossible d'ouvrir le fichier GPX pour modification !");
      }
    } else {
      // Si le fichier n'existe pas, le créer et ajouter l'en-tête GPX
      gpxFile = SD.open(gpxFilename, FILE_WRITE);
      if (gpxFile) {
        gpxFile.println("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
        gpxFile.println("<gpx version=\"1.1\" creator=\"GPS Logger\" xmlns=\"http://www.topografix.com/GPX/1/1\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:schemaLocation=\"http://www.topografix.com/GPX/1/1 http://www.topografix.com/GPX/1/1/gpx.xsd\">");
        gpxFile.println("<trk><name>GPS Log</name><trkseg>");
        Serial.println("Fichier GPX créé avec l'en-tête.");
      } else {
        Serial.println("Erreur : impossible de créer le fichier GPX !");
      }
    }

    fileCreated = true;  // Indiquer que les fichiers ont été créés
  }
}

/*
// Fonction pour créer ou ouvrir les fichiers de log (CSV et GPX)
void createLogFile() {
  if (sdAvailable) {
    Serial.println("Création des fichiers de log...");

    // Formater le nom du fichier CSV avec la date actuelle
    char csvFilename[40];
    sprintf(csvFilename, "/csv/%04d-%02d-%02d.csv", gps.date.year(), gps.date.month(), gps.date.day());

    // Ouvrir ou créer le fichier CSV en mode ajout
    logFile = SD.open(csvFilename, FILE_APPEND);
    if (!logFile) {
      logFile = SD.open(csvFilename, FILE_WRITE);
      logFile.println("Date,Heure,Latitude,Longitude,Vitesse(km/h),Altitude(m)");
      Serial.println("Fichier CSV créé.");
    }

    // Formater le nom du fichier GPX avec la date actuelle
    char gpxFilename[40];
    sprintf(gpxFilename, "/gpx/%04d-%02d-%02d.gpx", gps.date.year(), gps.date.month(), gps.date.day());

    // Vérifier si le fichier existe
    if (SD.exists(gpxFilename)) {
      // Si le fichier existe, ouvrir et supprimer les 3 dernières lignes
      gpxFile = SD.open(gpxFilename, FILE_APPEND);
      if (gpxFile) {
        removeLastThreeLines(gpxFilename);
      } else {
        Serial.println("Erreur : impossible d'ouvrir le fichier GPX pour modification !");
      }
    } else {
      // Si le fichier n'existe pas, le créer et ajouter l'en-tête GPX
      gpxFile = SD.open(gpxFilename, FILE_WRITE);
      if (gpxFile) {
        gpxFile.println("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
        gpxFile.println("<gpx version=\"1.1\" creator=\"GPS Logger\" xmlns=\"http://www.topografix.com/GPX/1/1\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:schemaLocation=\"http://www.topografix.com/GPX/1/1 http://www.topografix.com/GPX/1/1/gpx.xsd\">");
        gpxFile.println("<trk><name>GPS Log</name><trkseg>");
        Serial.println("Fichier GPX créé avec l'en-tête.");
      } else {
        Serial.println("Erreur : impossible de créer le fichier GPX !");
      }
    }

    fileCreated = true;  // Indiquer que les fichiers ont été créés
  }
}
*/
// Fonction pour journaliser les données GPS dans les fichiers CSV et GPX
void logGPSData() {
  if (!fileCreated && currentDate != "2000-00-00") {
    createLogFile();
  }

  if (sdAvailable && fileCreated && currentSatellites >= 4) {
    // Écrire les données dans le fichier CSV
    logFile.printf("%s,%s,%.6f,%.6f,%.2f,%.2f\n", currentDate.c_str(), currentTime.c_str(),
                   currentLatitude, currentLongitude, gps.speed.kmph(), currentAltitude);
    logFile.flush();  // Assurez-vous que les données sont bien écrites

    // Écrire les données dans le fichier GPX
    gpxFile.printf("<trkpt lat=\"%.6f\" lon=\"%.6f\">\n", currentLatitude, currentLongitude);
    gpxFile.printf("<ele>%.2f</ele>\n", currentAltitude);
    gpxFile.printf("<time>%sT%sZ</time>\n", currentDate.c_str(), currentTime.c_str());  // Format ISO 8601
    gpxFile.println("</trkpt>");
    gpxFile.flush();
  } else if (currentDate == "2000-00-00") {
    Serial.println("Date invalide, en attente de données GPS valides pour créer le fichier.");
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

#endif
