#ifndef HOME_H
#define HOME_H

#include "globals.h"
#include "button.h"  // Inclure le fichier button.h
#include "menu_fonctionnement.h"
#include "menu_bluetooth.h"
#include "menu_gps.h"
#include "menu_timer.h"
#include "menu_debug.h"
#include "menu_features.h"
#include "entete.h"

// Déclaration des boutons
Button btnRoulage = {65, 5, 122, 75, "Roulage"};
Button btnBluetooth = {192, 5, 122, 75, "Bluetooth"};
Button btnGPS = {65, 85, 122, 75, "GPS"};
Button btnTimer = {192, 85, 122, 75, "Timer"};
Button btnDebug = {65, 165, 122, 75, "Debug"};
Button btnFeatures = {192, 165, 122, 75, "Features"};

void displayHome() {
  currentPage = PAGE_HOME;
  tft.fillScreen(TFT_BLACK); // Effacer l'écran

  // Dessiner les boutons
  drawButton(tft, btnRoulage);
  drawButton(tft, btnBluetooth);
  drawButton(tft, btnGPS);
  drawButton(tft, btnTimer);
  drawButton(tft, btnDebug);
  drawButton(tft, btnFeatures);
}

void checkHomeTouch(uint16_t t_x, uint16_t t_y) {
  // Vérifier les appuis sur les boutons et changer de page si un bouton est pressé
  if (isButtonPressed(btnRoulage, t_x, t_y)) {
    displayFonctionnement();
    currentPage = PAGE_FONCTIONNEMENT;
  } else if (isButtonPressed(btnBluetooth, t_x, t_y)) {
    displayBluetooth();
    currentPage = PAGE_BLUETOOTH;
  } else if (isButtonPressed(btnGPS, t_x, t_y)) {
    displayGPS();
    currentPage = PAGE_GPS;
  } else if (isButtonPressed(btnTimer, t_x, t_y)) {
    displayTimer();
    currentPage = PAGE_TIMER;
  } else if (isButtonPressed(btnDebug, t_x, t_y)) {
    displayDebug();
    currentPage = PAGE_DEBUG;
  } else if (isButtonPressed(btnFeatures, t_x, t_y)) {
    displayFeatures();
    currentPage = PAGE_FEATURES;
  }
}

#endif
