#ifndef MENU_GPS_H
#define MENU_GPS_H

#include "entete.h"
#include "globals.h"

void displayGPS() {
  // Affichage du menu GPS
  tft.fillScreen(TFT_BLACK); // Effacer l'écran
  drawEntete();
  
  tft.setTextColor(TFT_WHITE);
  tft.setCursor(50, 50);
  tft.println("Menu GPS");

  currentPage = PAGE_GPS; // Mettre à jour la page courante
}

void checkGPSTouch(uint16_t t_x, uint16_t t_y) {
  // Gestion des appuis tactiles pour le menu GPS
  // Ajoute ici la gestion des interactions spécifiques
}

#endif
