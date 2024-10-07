#ifndef GESTION_TIMER_H
#define GESTION_TIMER_H

#include <SD.h>
#include <SPI.h>
#include <Arduino.h>
#include "globals.h"  // Inclure les variables GPS comme currentTime et currentDate

// Variables pour stocker les valeurs des timers en secondes
unsigned long timerAlexis = 0;
unsigned long timerRobin = 0;
unsigned long timerDaily = 0;
unsigned long timerTotal = 0;

// Variables pour stocker la dernière heure enregistrée
unsigned long previousTimeInSeconds = 0;

// Variable pour la gestion du conducteur actuel
extern String driverName;  // Utilise la variable globale "driverName" définie ailleurs

// Fonction pour convertir le temps en heures et minutes (format hhh:mm)
String formatTime(unsigned long seconds) {
  unsigned long hours = seconds / 3600;
  unsigned long minutes = (seconds % 3600) / 60;
  
  // Formatage avec 3 chiffres pour les heures et 2 chiffres pour les minutes
  char buffer[8];
  sprintf(buffer, "%03lu:%02lu", hours, minutes);
  return String(buffer);
}

// Fonction pour calculer l'heure en secondes (à partir d'une chaîne de type "hh:mm:ss")
unsigned long timeStringToSeconds(const String& timeStr) {
  int hour = timeStr.substring(0, 2).toInt();
  int minute = timeStr.substring(3, 5).toInt();
  int second = timeStr.substring(6, 8).toInt();
  return hour * 3600 + minute * 60 + second;
}

// Fonction pour récupérer la dernière heure enregistrée dans le CSV
unsigned long getLastLoggedTimeInSeconds() {
  unsigned long lastLoggedTimeInSeconds = 0;

  if (SD.exists("/timers.csv")) {
    File file = SD.open("/timers.csv");
    if (file) {
      String lastLine;
      while (file.available()) {
        lastLine = file.readStringUntil('\n');  // Lire chaque ligne jusqu'à la dernière
      }
      file.close();

      // Extraire l'heure de la dernière ligne et la convertir en secondes
      int spaceIndex = lastLine.indexOf(' ');
      String lastTimeStr = lastLine.substring(spaceIndex + 1, spaceIndex + 9);
      lastLoggedTimeInSeconds = timeStringToSeconds(lastTimeStr);
    }
  }
  return lastLoggedTimeInSeconds;
}

// Fonction pour calculer la différence de temps entre l'heure actuelle et la dernière enregistrée
unsigned long calculateElapsedTime() {
  unsigned long currentTimeInSeconds = gps.time.hour() * 3600 + gps.time.minute() * 60 + gps.time.second();
  unsigned long lastLoggedTimeInSeconds = getLastLoggedTimeInSeconds();

  // Si on est passé de l'autre côté de minuit
  if (currentTimeInSeconds < lastLoggedTimeInSeconds) {
    return (86400 - lastLoggedTimeInSeconds) + currentTimeInSeconds;
  }
  return currentTimeInSeconds - lastLoggedTimeInSeconds;
}

// Fonction pour ajouter le temps écoulé au bon timer
void updateTimers() {
  if (gps.time.isValid()) {
    // Calcul du temps écoulé
    unsigned long elapsedTime = calculateElapsedTime();

    // Ajouter ce temps aux timers du conducteur actuel
    if (driverName == "Alexis") {
      timerAlexis += elapsedTime;
    } else if (driverName == "Robin") {
      timerRobin += elapsedTime;
    }

    // Ajouter le temps écoulé aux timers "daily" et "total"
    timerDaily += elapsedTime;
    timerTotal += elapsedTime;

    // Mettre à jour l'heure précédente en secondes
    previousTimeInSeconds = gps.time.hour() * 3600 + gps.time.minute() * 60 + gps.time.second();
  }
}

// Fonction d'initialisation des timers
void initTimers() {
  previousTimeInSeconds = getLastLoggedTimeInSeconds();  // Initialiser avec la dernière heure du fichier

  if (!SD.exists("/timers.csv")) {
    // Si le fichier n'existe pas, le créer et initialiser les timers
    File file = SD.open("/timers.csv", FILE_WRITE);
    if (file) {
      file.println("Date Heure,Timer Alexis,Timer Robin,Timer Daily,Timer Total");
      file.close();
    }
    timerAlexis = 0;
    timerRobin = 0;
    timerDaily = 0;
    timerTotal = 0;
  }
}

// Fonction pour enregistrer les valeurs actuelles des timers dans le CSV toutes les 5 minutes
void logTimers() {
  File file = SD.open("/timers.csv", FILE_APPEND);
  if (file) {
    String dateTime = currentDate + " " + currentTime;  // Utiliser les variables GPS

    file.print(dateTime);
    file.print(',');
    file.print(formatTime(timerAlexis));
    file.print(',');
    file.print(formatTime(timerRobin));
    file.print(',');
    file.print(formatTime(timerDaily));
    file.print(',');
    file.println(formatTime(timerTotal));  // Ajoute un saut de ligne

    file.close();
  }
}

// Fonction pour réinitialiser les timers
void resetTimer(const String& timerName) {
  if (timerName == "Alexis") {
    timerAlexis = 0;
  } else if (timerName == "Robin") {
    timerRobin = 0;
  } else if (timerName == "Daily") {
    timerDaily = 0;
  } else if (timerName == "Total") {
    timerTotal = 0;
  }
}

#endif
