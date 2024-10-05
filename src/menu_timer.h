#ifndef MENU_TIMER_H
#define MENU_TIMER_H

#include "entete.h"
#include "globals.h"
#include "button.h"
int timezoneOffset = 2;

// Variables pour stocker les valeurs des timers (initialisées par défaut)
String timerValueAlexis = "000:00";
String timerValueRobin = "000:00";
String timerValueJournalier = "000:00";
String timerValueTotal = "000:00";

// Déclaration des boutons pour les noms des timers (sans action)
Button btnNameAlexis(64, 5, 60, 30, "Alexis");
Button btnNameRobin(128, 5, 60, 30, "Robin");
Button btnNameJournalier(192, 5, 60, 30, "Daily");
Button btnNameTotal(256, 5, 60, 30, "Total");

// Déclaration des boutons pour afficher les valeurs des timers (dynamique)
Button btnTimerAlexis(64, 40, 60, 30, timerValueAlexis.c_str());
Button btnTimerRobin(128, 40, 60, 30, timerValueRobin.c_str());
Button btnTimerJournalier(192, 40, 60, 30, timerValueJournalier.c_str());
Button btnTimerTotal(256, 40, 60, 30, timerValueTotal.c_str());

// Déclaration des boutons pour réinitialiser les timers
Button btnResetAlexis(64, 75, 60, 60, "Reset");      // Bouton pour réinitialiser le timer Alexis
Button btnResetRobin(128, 75, 60, 60, "Reset");      // Bouton pour réinitialiser le timer Robin
Button btnResetJournalier(192, 75, 60, 60, "Reset");  // Bouton pour réinitialiser le timer journalier
Button btnResetTotal(256, 75, 60, 60, "Reset");    // Bouton pour réinitialiser le timer Total

// Boutons pour la confirmation de réinitialisation
Button btnConfirmYes(70, 115, 115, 60, "Oui");
Button btnConfirmNo(195, 115, 115, 60, "Non");

// Boutons pour le décalage horaire
Button btnMinus1h(65, 180, 80, 60, "-1h");  // Bouton pour diminuer le décalage horaire
Button btnPlus1h(235, 180, 80, 60, "+1h");  // Bouton pour augmenter le décalage horaire

// Variables pour stocker quel timer doit être réinitialisé
enum TimerToReset {
  NONE,
  ALEXIS,
  ROBIN,
  JOURNALIER,
  TOTAL
} timerToReset = NONE;

// Fonction pour mettre à jour la valeur affichée des timers
void updateTimerValues() {

  // Met à jour les boutons avec les nouvelles valeurs des timers
  btnTimerAlexis.setLabel(timerValueAlexis.c_str());
  btnTimerRobin.setLabel(timerValueRobin.c_str());
  btnTimerJournalier.setLabel(timerValueJournalier.c_str());
  btnTimerTotal.setLabel(timerValueTotal.c_str());
}

// Affichage du menu Timer avec les boutons de noms, timers et réinitialisation
void displayTimer() {
  tft.fillScreen(TFT_BLACK); // Effacer l'écran
  drawEntete();

  // Mettre à jour les valeurs des timers avant de les afficher
  updateTimerValues();

  // Affichage des boutons pour les noms des timers (ligne du haut)
  drawButton(tft, btnNameAlexis);
  drawButton(tft, btnNameRobin);
  drawButton(tft, btnNameJournalier);
  drawButton(tft, btnNameTotal);

  // Affichage des boutons pour les valeurs des timers (ligne du milieu)
  drawButton(tft, btnTimerAlexis);
  drawButton(tft, btnTimerRobin);
  drawButton(tft, btnTimerJournalier);
  drawButton(tft, btnTimerTotal);

  // Affichage des boutons de réinitialisation sous chaque timer (ligne du bas)
  drawButton(tft, btnResetAlexis);
  drawButton(tft, btnResetRobin);
  drawButton(tft, btnResetJournalier);
  drawButton(tft, btnResetTotal);

  // Affichage des boutons -1h et +1h en bas
  drawButton(tft, btnMinus1h);

  // Afficher le décalage horaire au centre entre les boutons
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);
  tft.setCursor(165, 230);  // Ajuster l'emplacement pour centrer le texte
  tft.printf("%+02dh", timezoneOffset);  // Affichage de l'offset actuel
  
  drawButton(tft, btnPlus1h);

  // Mettre à jour la page courante
  currentPage = PAGE_TIMER;
}

// Afficher une page de confirmation pour la réinitialisation
void displayConfirmation(const char* timerName) {
  tft.fillScreen(TFT_BLACK); // Effacer l'écran
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(1);
  tft.setCursor(70, 40);
  tft.printf("Reinitialiser %s ?", timerName);  // Afficher un message de confirmation
  
  // Afficher les boutons de confirmation Oui et Non
  drawButton(tft, btnConfirmYes);
  drawButton(tft, btnConfirmNo);

  // Changer la page courante pour la confirmation
  currentPage = PAGE_CONFIRMATION;
}

// Gestion des interactions tactiles pour la confirmation
void checkConfirmationTouch(uint16_t t_x, uint16_t t_y) {
  if (isButtonPressed(btnConfirmYes, t_x, t_y)) {
    // Réinitialiser le timer en fonction de la sélection
    switch (timerToReset) {
      case ALEXIS:
        Serial.println("Timer Alexis réinitialisé");
        break;
      case ROBIN:
        Serial.println("Timer Robin réinitialisé");
        break;
      case JOURNALIER:
        Serial.println("Timer Journalier réinitialisé");
        break;
      case TOTAL:
        Serial.println("Timer Total réinitialisé");
        break;
      default:
        break;
    }
    timerToReset = NONE;
    displayTimer();  // Retour au menu principal
  } else if (isButtonPressed(btnConfirmNo, t_x, t_y)) {
    Serial.println("Réinitialisation annulée");
    timerToReset = NONE;
    displayTimer();  // Retour au menu principal
  }
}

// Gestion des interactions tactiles
void checkTimerTouch(uint16_t t_x, uint16_t t_y) {
  // Vérifier si un bouton a été pressé et gérer les interactions correspondantes
  if (isButtonPressed(btnResetAlexis, t_x, t_y)) {
    Serial.println("Demande de réinitialisation du timer Alexis");
    timerToReset = ALEXIS;
    displayConfirmation("Alexis");
  } else if (isButtonPressed(btnResetRobin, t_x, t_y)) {
    Serial.println("Demande de réinitialisation du timer Robin");
    timerToReset = ROBIN;
    displayConfirmation("Robin");
  } else if (isButtonPressed(btnResetJournalier, t_x, t_y)) {
    Serial.println("Demande de réinitialisation du timer Journalier");
    timerToReset = JOURNALIER;
    displayConfirmation("Journalier");
  } else if (isButtonPressed(btnResetTotal, t_x, t_y)) {
    Serial.println("Demande de réinitialisation du timer Total");
    timerToReset = TOTAL;
    displayConfirmation("Total");
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
