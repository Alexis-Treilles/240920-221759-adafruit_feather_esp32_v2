#ifndef GLOBALS_H
#define GLOBALS_H

// Enumération des différentes pages

enum Page {
  PAGE_HOME,
  PAGE_FONCTIONNEMENT,
  PAGE_BLUETOOTH,
  PAGE_GPS,
  PAGE_TIMER,
  PAGE_DEBUG,
  PAGE_FEATURES
};
// Variables globales à utiliser dans tout le programme
extern int currentTemperature;   // Température actuelle sous forme d'entier
extern bool deviceConnected;     // Statut de la connexion Bluetooth
extern bool buttonPreviousPressed;
extern bool buttonPlayPausePressed;
extern bool buttonNextPressed;

extern String currentTime;       // Heure actuelle sous forme de chaîne
extern String currentDate;
extern int currentSatellites;    // Nombre de satellites actuels
extern float currentLatitude;
extern float currentLongitude;  
extern int timezoneOffset;  // Déclaration de la variable globale timezoneOffset
extern double currentAltitude;

// Variable globale pour stocker la page actuelle
extern Page currentPage;

// Inclure tous les menus ici pour que les fonctions soient accessibles
#include "home.h"
#include "menu_fonctionnement.h"
#include "menu_bluetooth.h"
#include "menu_gps.h"
#include "menu_timer.h"

#endif
