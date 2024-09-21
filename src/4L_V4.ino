#include <TFT_eSPI.h>
#include "globals.h"      // Inclure les variables globales
#include "home.h"
#include "menu_fonctionnement.h"
#include "menu_bluetooth.h"
#include "menu_gps.h"
#include "menu_timer.h"
#include "entete.h"
#include "temp.h"
#include "gps.h"    // Inclure le fichier pour la mise à jour des données GPS
#include <TinyGPS++.h>     // Bibliothèque GPS

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

// Définir la variable globale pour stocker la page courante
Page currentPage = PAGE_HOME;

// Créer une instance de TinyGPS++
TinyGPSPlus gps;

// Déclarer le port série pour la communication avec le module GPS (définir les broches selon ton câblage)
HardwareSerial SerialGPS(1);
#define RXPin 16
#define TXPin 17

unsigned long previousMillisDebug = 0;
unsigned long previousMillisTemp = 0;
unsigned long previousMillisGPS = 0;
const long intervalDebug = 1000;   // Intervalle pour actualiser le menu debug
const long intervalTemp = 10000;   // Intervalle pour actualiser la température
const long intervalGPS = 5000;     // Intervalle pour actualiser les données GPS toutes les 5 secondes

void setup() {
  tft.init();
  tft.setRotation(3);    // Orientation horizontale
  tft.fillScreen(TFT_BLACK);
  tft.setFreeFont(&FreeSerif9pt7b);  // Charger une police plus petite
  Serial.begin(115200);  // Démarrer la communication série pour le débogage
  initBluetooth();
  sensors.begin();       // Initialiser le capteur DS18B20
  SerialGPS.begin(9600, SERIAL_8N1, RXPin, TXPin);  // Initialiser le GPS
  displayHome();         // Afficher le menu home par défaut
}

void loop() {
  uint16_t t_x = 0, t_y = 0; // Pour stocker les coordonnées tactiles
  bool pressed = tft.getTouch(&t_x, &t_y);
  
  if (pressed) {
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

  // Mettre à jour les données GPS toutes les 5 secondes
  if (currentMillis - previousMillisGPS >= intervalGPS) {
    previousMillisGPS = currentMillis;

    // Lire les données GPS
    while (SerialGPS.available() > 0) {
      gps.encode(SerialGPS.read());
    }

    // Mettre à jour les informations GPS
    updateGPSData();
  }
}
