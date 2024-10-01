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

Page currentPage = PAGE_HOME;

TinyGPSPlus gps;
HardwareSerial SerialGPS(1);


unsigned long previousMillisDebug = 0;
unsigned long previousMillisTemp = 0;
unsigned long previousMillisGPSAndLog = 0;  // Utilisé pour la mise à jour GPS et log

const long intervalDebug = 1000;   // Intervalle pour actualiser le menu debug
const long intervalTemp = 10000;   // Intervalle pour actualiser la température
const long intervalGPSAndLog = 5000;  // Intervalle combiné pour GPS et log (toutes les 5 secondes)

void setup() {
  Serial.begin(115200);  // Initialiser la communication série
  tft.init();            // Initialiser l'écran TFT
  tft.setRotation(3);    // Orientation de l'écran
  bootSetup();           // Appel de la fonction de démarrage dans boot.h
}
 uint16_t t_x, t_y; 
void loop() {
 // Pour stocker les coordonnées tactiles
  
  
  //Serial.println("Avant getTouch");
  if (tft.getTouch(&t_x, &t_y)) {
    Serial.println("Touch détecté !");
    Serial.print("Coordonnées touchées : X = ");
    Serial.print(t_x);
    Serial.print(", Y = ");
    Serial.println(t_y);
    t_x = tft.width() - t_x;
    checkEnteteTouch(t_x, t_y);

    if (currentPage == PAGE_HOME) {
      checkHomeTouch(t_x, t_y);
    } else if (currentPage == PAGE_FONCTIONNEMENT) {
      checkFonctionnementTouch(t_x, t_y);
    } else if (currentPage == PAGE_BLUETOOTH) {
      checkBluetoothTouch(t_x, t_y);
    } else if (currentPage == PAGE_GPS) {
      checkGPSTouch(t_x, t_y);
    } else if (currentPage == PAGE_TIMER) {
      checkTimerTouch(t_x, t_y);
    } else if (currentPage == PAGE_DEBUG) {
      checkDebugTouch(t_x, t_y);
    }
  }

  // Mettre à jour l'entête en continu
  updateGPSData();
  drawEntete();

  unsigned long currentMillis = millis();

  // Actualisation spécifique pour le menu Debug
  if (currentPage == PAGE_DEBUG) {
    if (currentMillis - previousMillisDebug >= intervalDebug) {
      previousMillisDebug = currentMillis;
      displayDebug();  // Appel de la fonction pour rafraîchir l'affichage dans le menu debug
    }
  }

  // Mettre à jour la température toutes les 10 secondes
  if (currentMillis - previousMillisTemp >= intervalTemp) {
    previousMillisTemp = currentMillis;
    updateTemperature();  // Mettre à jour la température
  }

  // Mettre à jour les données GPS et enregistrer les logs toutes les 5 secondes
  if (currentMillis - previousMillisGPSAndLog >= intervalGPSAndLog) {
    previousMillisGPSAndLog = currentMillis;
    processAndLogGPSData();
  }
}
