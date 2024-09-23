#ifndef MENU_TIMER_H
#define MENU_TIMER_H

#include "entete.h"
#include "globals.h"
#include "button.h"
int timezoneOffset =2;
// Déclaration des boutons pour réinitialiser les timers
Button btnResetTotal(65, 70, 80, 60, "Reset");      // Bouton pour réinitialiser le timer total
Button btnResetJournalier(150, 70, 80, 60, "Reset");  // Bouton pour réinitialiser le timer journalier
Button btnResetManuel(235, 70, 80, 60, "Reset");    // Bouton pour réinitialiser le timer manuel

// Boutons pour le décalage horaire
Button btnMinus1h(65, 180, 80, 60, "-1h");  // Bouton pour diminuer le décalage horaire
Button btnPlus1h(235, 180, 80, 60, "+1h");  // Bouton pour augmenter le décalage horaire

// Affichage du menu Timer avec les 3 timers et les boutons de décalage horaire
void displayTimer() {
  tft.fillScreen(TFT_BLACK); // Effacer l'écran
  
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(1);

  // Encadrés pour les timers
  tft.drawRoundRect(65, 5, 80, 60, 10, TFT_WHITE);   // Encadré pour le timer total
  tft.drawRoundRect(150, 5, 80, 60, 10, TFT_WHITE);  // Encadré pour le timer journalier
  tft.drawRoundRect(235, 5, 80, 60, 10, TFT_WHITE);  // Encadré pour le timer manuel

  tft.setCursor(70, 40);  // Position pour le timer total
  tft.printf("%03d:%02d", 0, 0);  // Remplacer par la valeur du timer total

  tft.setCursor(155, 40);  // Position pour le timer journalier
  tft.printf("%03d:%02d", 0, 0);  // Remplacer par la valeur du timer journalier

  tft.setCursor(240, 40);  // Position pour le timer manuel
  tft.printf("%03d:%02d", 0, 0);  // Remplacer par la valeur du timer manuel

  // Affichage des boutons de réinitialisation sous chaque timer
  drawButton(tft, btnResetTotal);
  drawButton(tft, btnResetJournalier);
  drawButton(tft, btnResetManuel);

  // Affichage des boutons -1h et +1h en bas
  drawButton(tft, btnMinus1h);

  // Afficher le décalage horaire au centre entre les boutons
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);
  tft.setCursor(165, 200);  // Ajuster l'emplacement pour centrer le texte
  tft.printf("%+02dh", timezoneOffset);  // Affichage de l'offset actuel
  
  drawButton(tft, btnPlus1h);

  // Mettre à jour la page courante
  currentPage = PAGE_TIMER;
}

// Gestion des interactions tactiles
void checkTimerTouch(uint16_t t_x, uint16_t t_y) {
  // Vérifier si un bouton a été pressé et gérer les interactions correspondantes
  if (isButtonPressed(btnResetTotal, t_x, t_y)) {
    // Réinitialiser le timer total (fonctionnalité à implémenter)
    Serial.println("Bouton 'Reset Total' pressé");
  } else if (isButtonPressed(btnResetJournalier, t_x, t_y)) {
    // Réinitialiser le timer journalier (fonctionnalité à implémenter)
    Serial.println("Bouton 'Reset Journalier' pressé");
  } else if (isButtonPressed(btnResetManuel, t_x, t_y)) {
    // Réinitialiser le timer manuel (fonctionnalité à implémenter)
    Serial.println("Bouton 'Reset Manuel' pressé");
  } else if (isButtonPressed(btnMinus1h, t_x, t_y)) {
    // Diminuer le décalage horaire
    if (timezoneOffset > -12) {  // Limite inférieure pour le décalage horaire
      timezoneOffset--;
      Serial.printf("Décalage horaire : %+02dh\n", timezoneOffset);
    }
    displayTimer();  // Mettre à jour l'affichage
  } else if (isButtonPressed(btnPlus1h, t_x, t_y)) {
    // Augmenter le décalage horaire
    if (timezoneOffset < 12) {  // Limite supérieure pour le décalage horaire
      timezoneOffset++;
      Serial.printf("Décalage horaire : %+02dh\n", timezoneOffset);
    }
    displayTimer();  // Mettre à jour l'affichage
  }
}

#endif
