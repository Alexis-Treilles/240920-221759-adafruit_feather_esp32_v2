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

// Fonction pour ajouter du temps au bon timer
void updateTimers() {
  // Ajoute du temps au timer correspondant
  if (driverName == "Alexis") {
    timerAlexis += 300;  // Ajoute 5 minutes (300 secondes)
  } else if (driverName == "Robin") {
    timerRobin += 300;   // Ajoute 5 minutes (300 secondes)
  }

  // Ajoute du temps aux timers "daily" et "total"
  timerDaily += 300;
  timerTotal += 300;
}

// Fonction d'initialisation des timers
void initTimers() {
  // Vérifie si le fichier CSV existe déjà
  if (!SD.exists("/timers.csv")) {
    // Si le fichier n'existe pas, on le crée
    File file = SD.open("/timers.csv", FILE_WRITE);
    if (file) {
      // Écrire l'en-tête dans le CSV
      file.println("Date Heure,Timer Alexis,Timer Robin,Timer Daily,Timer Total");
      file.close();
    }
    // Initialisation des timers à 0
    timerAlexis = 0;
    timerRobin = 0;
    timerDaily = 0;
    timerTotal = 0;
  } else {
    // Si le fichier existe, on récupère la dernière ligne pour initialiser les timers
    File file = SD.open("/timers.csv");
    if (file) {
      String lastLine;
      while (file.available()) {
        lastLine = file.readStringUntil('\n');  // Lire chaque ligne jusqu'à la dernière
      }
      file.close();

      // Extraire les valeurs de la dernière ligne et les convertir en entiers
      int lastComma1 = lastLine.indexOf(',');
      int lastComma2 = lastLine.indexOf(',', lastComma1 + 1);
      int lastComma3 = lastLine.indexOf(',', lastComma2 + 1);
      int lastComma4 = lastLine.indexOf(',', lastComma3 + 1);

      // Conversion des sous-chaînes en valeurs numériques pour chaque timer
      timerAlexis = lastLine.substring(lastComma1 + 1, lastComma2).toInt();
      timerRobin = lastLine.substring(lastComma2 + 1, lastComma3).toInt();
      timerDaily = lastLine.substring(lastComma3 + 1, lastComma4).toInt();
      timerTotal = lastLine.substring(lastComma4 + 1).toInt();
    }
  }
}

// Fonction pour enregistrer les valeurs actuelles des timers dans le CSV toutes les 5 minutes
void logTimers() {
  // Ouvre le fichier CSV en mode ajout (append)
  File file = SD.open("/timers.csv", FILE_APPEND);
  if (file) {
    // Utiliser la date et l'heure actuelles du GPS
    String dateTime = currentDate + " " + currentTime;  // Utiliser les variables GPS

    // Écrire la date, l'heure et les valeurs des timers dans le CSV
    file.print(dateTime);
    file.print(',');
    file.print(formatTime(timerAlexis));
    file.print(',');
    file.print(formatTime(timerRobin));
    file.print(',');
    file.print(formatTime(timerDaily));
    file.print(',');
    file.println(formatTime(timerTotal));  // Ajoute un saut de ligne pour la prochaine entrée

    file.close();  // Fermer le fichier après l'écriture
  }
}

// Fonction pour réinitialiser les timers
void resetTimer(const String &timerName) {
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
