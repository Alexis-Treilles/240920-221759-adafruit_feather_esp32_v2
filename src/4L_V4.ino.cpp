# 1 "C:\\Users\\alexi\\AppData\\Local\\Temp\\tmp78uccaqo"
#include <Arduino.h>
# 1 "D:/Documents/VS_code_projects/240920-221759-adafruit_feather_esp32_v2/src/4L_V4.ino"
#include "TFT_eSPI.h"
#include "globals.h"
#include "home.h"
#include "menu_fonctionnement.h"
#include "menu_bluetooth.h"
#include "menu_gps.h"
#include "menu_timer.h"
#include "entete.h"
#include "temp.h"
#include "gps.h"
#include <TinyGPS++.h>
#include "log.h"
#include "gestion_timer.h"


OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

TFT_eSPI tft = TFT_eSPI();
String currentTime = "00:00";
int currentSatellites = 0;
bool buttonPreviousPressed = false;
bool buttonPlayPausePressed = false;
bool buttonNextPressed = false;
bool deviceConnected = false;
int currentTemperature = 99;
float currentLatitude = 0.0;
float currentLongitude = 0.0;
float currentSpeed = 0.0;
uint16_t t_x, t_y;

Page currentPage = PAGE_HOME;

TinyGPSPlus gps;
HardwareSerial SerialGPS(1);

unsigned long previousMillisDebug = 0;
unsigned long previousMillisTemp = 0;
unsigned long previousMillisGPSAndLog = 0;
unsigned long previousMillisTimers = 0;

const long intervalDebug = 1000;
const long intervalTemp = 10000;
const long intervalGPSAndLog = 5000;
const long intervalTimers = 60000;
void setup();
void loop();
#line 47 "D:/Documents/VS_code_projects/240920-221759-adafruit_feather_esp32_v2/src/4L_V4.ino"
void setup() {
  Serial.begin(115200);
  initScreenAndTouch();
  bootSetup();
}



void loop() {
  unsigned long currentMillis = millis();

  sdAvailable = restartSD();



  sdWasAvailable = sdAvailable;



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

  }


  if (currentMillis - previousMillisDebug >= 1000) {

    previousMillisDebug = currentMillis;
    drawEntete();
  }


  if (currentMillis - previousMillisGPSAndLog >= intervalGPSAndLog) {

    previousMillisGPSAndLog = currentMillis;
    processAndLogGPSData();
  }


  if (currentPage == PAGE_DEBUG) {
    if (currentMillis - previousMillisDebug >= intervalDebug) {

      previousMillisDebug = currentMillis;
      displayDebug();
    }
  }


  if (currentMillis - previousMillisTemp >= intervalTemp) {

    previousMillisTemp = currentMillis;
    updateTemperature();
  }

  if (currentMillis - previousMillisTimers >= intervalTimers) {
    previousMillisTimers = currentMillis;
    updateTimers();
    logTimers();
    updateGPSData();
  }
}