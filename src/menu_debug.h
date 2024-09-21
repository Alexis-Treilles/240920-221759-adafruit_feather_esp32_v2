#ifndef MENU_DEBUG_H
#define MENU_DEBUG_H

#include "globals.h"
#include "entete.h"
void displayDebug() {
  currentPage = PAGE_DEBUG;
  tft.fillScreen(TFT_BLACK); // Effacer l'écran
  drawEntete();              // Afficher l'entête
  
  // Texte Debug
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);
  tft.setCursor(80, 100);
  tft.println("Mode Debug");

  // Afficher quelques informations de debug, par exemple :
  tft.setTextSize(1);
  tft.setCursor(80, 140);
  tft.println("Heure: " + currentTime);
  tft.setCursor(80, 160);
  tft.println("Satellites: " + String(currentSatellites));
  tft.setCursor(80, 180);
  tft.println("Température: " + String(currentTemperature) + "°C");
}

// Fonction pour gérer les appuis tactiles dans le menu Debug
void checkDebugTouch(uint16_t t_x, uint16_t t_y) {
  // Ici, tu peux gérer les interactions tactiles spécifiques au menu Debug
}

#endif