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

void setup() {
  Serial.begin(115200);  // Initialiser la communication série
  initScreenAndTouch();  // Initialiser l'écran et faire la calibration tactile
  bootSetup();           // Appel de la fonction de démarrage
}



void loop() {
  unsigned long currentMillis = millis();
  // Vérification en continu de la disponibilité de la carte SD
  sdAvailable = restartSD();


  // Mise à jour de l'état précédent de la carte SD
  sdWasAvailable = sdAvailable;
  
  
  // Vérification tactile
  if (tft.getTouch(&t_x, &t_y,200)) {
    Serial.println("Touch détecté !");
    Serial.print("Coordonnées touchées : X = ");
    Serial.print(t_x);
    Serial.print(", Y = ");
    Serial.println(t_y);
    checkEnteteTouch(t_x, t_y);

    switch (currentPage) {
      case PAGE_HOME:
        Serial.println("Touch sur PAGE_HOME");
        checkHomeTouch(t_x, t_y);
        break;
      case PAGE_FONCTIONNEMENT:
        Serial.println("Touch sur PAGE_FONCTIONNEMENT");
        checkFonctionnementTouch(t_x, t_y);
        break;
      case PAGE_BLUETOOTH:
        Serial.println("Touch sur PAGE_BLUETOOTH");
        checkBluetoothTouch(t_x, t_y);
        break;
      case PAGE_GPS:
        Serial.println("Touch sur PAGE_GPS");
        checkGPSTouch(t_x, t_y);
        break;
      case PAGE_TIMER:
        Serial.println("Touch sur PAGE_TIMER");
        checkTimerTouch(t_x, t_y);
        break;
      case PAGE_DEBUG:
        Serial.println("Touch sur PAGE_DEBUG");
        checkDebugTouch(t_x, t_y);
      case PAGE_CONFIRMATION:
        Serial.println("Touch sur PAGE_CONFIRMATION");
        checkConfirmationTouch (t_x, t_y);
        break;
      default:
        Serial.println("Touch sur une page inconnue !");
        break;
    }
  } else {
    //Serial.println("Aucun touch détecté.");
  }

  // Mettre à jour l'entête toutes les secondes
  if (currentMillis - previousMillisDebug >= 1000) {  // 1000 ms = 1 seconde
    //Serial.println("Mise à jour de l'entête...");
    previousMillisDebug = currentMillis;
    drawEntete();  // Appel de la fonction pour dessiner l'entête
  }

  // Mettre à jour les données GPS toutes les 5 secondes
  if (currentMillis - previousMillisGPSAndLog >= intervalGPSAndLog) {
    //Serial.println("Mise à jour des données GPS...");
    previousMillisGPSAndLog = currentMillis;
    processAndLogGPSData();  // Mettre à jour les données GPS et enregistrer les logs
  }

  // Actualisation spécifique pour le menu Debug
  if (currentPage == PAGE_DEBUG) {
    if (currentMillis - previousMillisDebug >= intervalDebug) {
      //Serial.println("Mise à jour du menu Debug...");
      previousMillisDebug = currentMillis;
      displayDebug();  // Appel de la fonction pour rafraîchir l'affichage dans le menu debug
    }
  }

  // Mettre à jour la température toutes les 10 secondes
  if (currentMillis - previousMillisTemp >= intervalTemp) {
    //Serial.println("Mise à jour de la température...");
    previousMillisTemp = currentMillis;
    updateTemperature();  // Mettre à jour la température
  }
  // Mettre à jour et enregistrer les timers toutes les 5 minutes
  if (currentMillis - previousMillisTimers >= intervalTimers) {
    previousMillisTimers = currentMillis;
    updateTimers();  // Mettre à jour les timers (en fonction du conducteur actuel)
    logTimers();     // Enregistrer les valeurs actuelles des timers dans le fichier CSV
    updateGPSData(); // Mettre à jour et enregistration
  }
}
