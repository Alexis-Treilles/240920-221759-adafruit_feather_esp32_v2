# 1 "C:\\Users\\X1-ALE~1\\AppData\\Local\\Temp\\tmpjoypnuy6"
#include <Arduino.h>
# 1 "C:/Users/X1-Alexis/Documents/PlatformIO/Projects/240920-221759-adafruit_feather_esp32_v2/src/4L_V4.ino"
#include <TFT_eSPI.h>
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

Page currentPage = PAGE_HOME;

TinyGPSPlus gps;
HardwareSerial SerialGPS(1);
#define RXPin 16
#define TXPin 17

unsigned long previousMillisDebug = 0;
unsigned long previousMillisTemp = 0;
unsigned long previousMillisGPSAndLog = 0;

const long intervalDebug = 1000;
const long intervalTemp = 10000;
const long intervalGPSAndLog = 5000;
void setup();
void loop();
#line 44 "C:/Users/X1-Alexis/Documents/PlatformIO/Projects/240920-221759-adafruit_feather_esp32_v2/src/4L_V4.ino"
void setup() {
  tft.init();
  tft.setRotation(3);
  tft.fillScreen(TFT_BLACK);
  tft.setFreeFont(&FreeSerif9pt7b);
  Serial.begin(115200);
  initBluetooth();
  sensors.begin();
  SerialGPS.begin(9600, SERIAL_8N1, RXPin, TXPin);
  displayHome();


  initLogFile(gps.date.day(), gps.date.month(), gps.date.year());
}

void loop() {
  uint16_t t_x = 0, t_y = 0;
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


  drawEntete();

  unsigned long currentMillis = millis();


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


  if (currentMillis - previousMillisGPSAndLog >= intervalGPSAndLog) {
    previousMillisGPSAndLog = currentMillis;
    processAndLogGPSData(gps, SerialGPS);
  }
}