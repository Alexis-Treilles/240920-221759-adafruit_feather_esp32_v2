#ifndef MENU_FEATURES_H
#define MENU_FEATURES_H

#include "globals.h"
#include "entete.h"

void displayFeatures() {
  currentPage = PAGE_FEATURES;
  tft.fillScreen(TFT_BLACK); // Effacer l'écran
  drawEntete();              // Afficher l'entête
  
  // Texte Features
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);
  tft.setCursor(80, 100);
  tft.println("Fonctionnalites");

  // Afficher quelques informations sur les fonctionnalités
  tft.setTextSize(1);
  tft.setCursor(80, 140);
  tft.println("Fonction 1: ");
  tft.setCursor(80, 160);
  tft.println("Fonction 2: ");
  tft.setCursor(80, 180);
  tft.println("Fonction 3: ");
}

// Fonction pour gérer les appuis tactiles dans le menu Features
void checkFeaturesTouch(uint16_t t_x, uint16_t t_y) {
  // Ici, tu peux gérer les interactions tactiles spécifiques au menu Features
}

#endif
