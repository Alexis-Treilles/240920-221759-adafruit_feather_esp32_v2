#include "TFT_eSPI.h"  // Inclure la bibliothèque TFT_eSPI
#include "globals.h"      // Inclure les variables globales
#include "home.h"         // Inclure le menu principal
#include "menu_fonctionnement.h"   // Inclure le menu Fonctionnement
#include "menu_bluetooth.h"    // Inclure le menu Bluetooth
#include "menu_gps.h"   // Inclure le menu GPS
#include "menu_timer.h" // Inclure le menu Timer
#include "entete.h"  // Inclure l'entête
#include "temp.h"    // Inclure le fichier pour la mise à jour des données de température
#include "gps.h"    // Inclure le fichier pour la mise à jour des données GPS
#include <TinyGPS++.h>     // Bibliothèque GPS
#include "log.h"           // Inclure le fichier log pour la journalisation
#include "gestion_timer.h" // Inclure la gestion des timers (ajout)
#include "boot.h" // Inclure le fichier

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

TFT_eSPI tft = TFT_eSPI();
String currentTime = "00:00";   // Exemple d'heure initiale (à remplacer par celle du GPS)
int currentSatellites = 0;        // Nombre de satellites par défaut
bool buttonPreviousPressed = false;
bool buttonPlayPausePressed = false;
bool buttonNextPressed = false;
bool deviceConnected = false;
int currentTemperature = 99;    // Valeur par défaut pour la température (ex. : -127 pour indiquer que le capteur n'a pas encore donné de valeur valide)
float currentLatitude = 0.0;    // Latitude par défaut
float currentLongitude = 0.0;   // Longitude par défaut
float currentSpeed = 0.0;       // Vitesse par défaut
uint16_t t_x, t_y;

Page currentPage = PAGE_HOME;

TinyGPSPlus gps;
HardwareSerial SerialGPS(1);

unsigned long previousMillisDebug = 0;
unsigned long previousMillisTemp = 0;
unsigned long previousMillisGPSAndLog = 0;  // Utilisé pour la mise à jour GPS et log
unsigned long previousMillisTimers = 0;  // Utilisé pour l'enregistrement des timers dans le CSV
bool sdWasAvailable = true;
const long intervalDebug = 1000;   // Intervalle pour actualiser le menu debug
const long intervalTemp = 10000;   // Intervalle pour actualiser la température
const long intervalGPSAndLog = 5000;  // Intervalle combiné pour GPS et log (toutes les 5 secondes)
const long intervalTimers = 60000;  // Intervalle pour l'enregistrement des timers (5 minutes)

// Prototypes des tâches
void taskCore0(void * parameter);
void taskCore1(void * parameter);

// Fonction d'initialisation
void setup() {
  Serial.begin(115200);  // Initialiser la communication série
  
  // Initialisation de l'écran et du tactile
  initScreenAndTouch();
  bootSetup();

  // Création des tâches pour Core 0 (GPS, Logs, Timers, Capteurs)
  xTaskCreatePinnedToCore(
    taskCore0,    // Fonction de la tâche pour le Core 0
    "Core 0 Task",    // Nom de la tâche
    10000,       // Taille de la pile
    NULL,        // Paramètre passé à la tâche
    1,           // Priorité de la tâche
    NULL,        // Handle de la tâche
    0            // Exécuter sur Core 0
  );

  // Création des tâches pour Core 1 (Affichage TFT et Bluetooth)
  xTaskCreatePinnedToCore(
    taskCore1,    // Fonction de la tâche pour le Core 1
    "Core 1 Task",   // Nom de la tâche
    10000,       // Taille de la pile
    NULL,        // Paramètre passé à la tâche
    1,           // Priorité de la tâche
    NULL,        // Handle de la tâche
    1            // Exécuter sur Core 1
  );
}

void loop() {
  // Rien dans la boucle principale car tout est géré par les tâches
}

// Tâche pour le Core 0 : GPS, Log, Timers, Capteurs
void taskCore0(void * parameter) {
  unsigned long previousMillisGPSAndLog = 0;
  unsigned long previousMillisTemp = 0;
  unsigned long previousMillisTimers = 0;

  while (true) {
    unsigned long currentMillis = millis();

    // Mise à jour GPS, log et enregistrement des timers toutes les 5 secondes
    if (currentMillis - previousMillisGPSAndLog >= intervalGPSAndLog) {
      previousMillisGPSAndLog = currentMillis;
      processAndLogGPSData();
    }

    // Mise à jour des capteurs (ex. température) toutes les 10 secondes
    if (currentMillis - previousMillisTemp >= intervalTemp) {
      previousMillisTemp = currentMillis;
      updateTemperature();
    }

    // Mise à jour et journalisation des timers toutes les 5 minutes
    if (currentMillis - previousMillisTimers >= intervalTimers) {
      previousMillisTimers = currentMillis;
      updateTimers();
      logTimers();     // Enregistrer les valeurs actuelles des timers dans le fichier CSV
    }

    // FreeRTOS gère la répartition des tâches donc pas besoin de delay
  }
}

// Tâche pour le Core 1 : Affichage TFT et Bluetooth
void taskCore1(void * parameter) {
  unsigned long previousMillisDebug = 0;

  while (true) {
    unsigned long currentMillis = millis();

    // Vérification des interactions tactiles
    if (tft.getTouch(&t_x, &t_y, 200)) {
      checkEnteteTouch(t_x, t_y);
      switch (currentPage) {
        case PAGE_HOME:
          checkHomeTouch(t_x, t_y);
          break;
        case PAGE_BLUETOOTH:
          checkBluetoothTouch(t_x, t_y);
          break;
        case PAGE_GPS:
          checkGPSTouch(t_x, t_y);
          break;
        case PAGE_TIMER:
          checkTimerTouch(t_x, t_y);
          break;
        case PAGE_DEBUG:
          checkDebugTouch(t_x, t_y);
          break;
        case PAGE_CONFIRMATION:
          checkConfirmationTouch(t_x, t_y);
          break;
        default:
          Serial.println("Touch sur une page inconnue !");
          break;
      }
    }

    // Mise à jour de l'affichage toutes les secondes
    if (currentMillis - previousMillisDebug >= intervalDebug) {
      previousMillisDebug = currentMillis;
      drawEntete();
    }

    // FreeRTOS gère la répartition des tâches donc pas besoin de delay
  }
}
