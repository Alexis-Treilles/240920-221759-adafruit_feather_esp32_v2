#ifndef MENU_FONCTIONNEMENT_H
#define MENU_FONCTIONNEMENT_H

#include "entete.h"
#include "globals.h"

void displayFonctionnement() {
  // Affichage du menu fonctionnement
  tft.fillScreen(TFT_BLACK); // Effacer l'écran  
  drawEntete();
  tft.setTextColor(TFT_WHITE);
  tft.setCursor(50, 50);
  tft.println("Menu Fonctionnement");

  currentPage = PAGE_FONCTIONNEMENT; // Mettre à jour la page courante
}

void checkFonctionnementTouch(uint16_t t_x, uint16_t t_y) {
  // Gestion des appuis tactiles pour le menu fonctionnement
  // Ajoute ici la gestion des interactions spécifiques
}

#endif
