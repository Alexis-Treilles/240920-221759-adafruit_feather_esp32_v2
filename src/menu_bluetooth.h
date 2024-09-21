#ifndef MENU_BLUETOOTH_H
#define MENU_BLUETOOTH_H

#include "entete.h"
#include "globals.h"
#include "button.h"
#include "gestion_bluetooth.h"  // Inclure pour accéder à checkBluetoothConnection()

// Largeur de la sidebar à gauche
#define SIDEBAR_WIDTH 60

// Positions et tailles des boutons
#define BUTTON_W 80  // Largeur des boutons
#define BUTTON_HEIGHT 50  // Hauteur des boutons
#define BUTTON_SPACING 5  // Espacement entre les boutons

// Position des boutons
#define BUTTON_X_START 65
#define BUTTON_Y_START 180

// Déclaration des boutons
Button btnPrevious(BUTTON_X_START, BUTTON_Y_START, BUTTON_W, BUTTON_HEIGHT, "Précédent");
Button btnPlayPause(BUTTON_X_START + BUTTON_W + BUTTON_SPACING, BUTTON_Y_START, BUTTON_W, BUTTON_HEIGHT, "Play/Pause");
Button btnNext(BUTTON_X_START + 2 * (BUTTON_W + BUTTON_SPACING), BUTTON_Y_START, BUTTON_W, BUTTON_HEIGHT, "Suivant");

// Fonction pour afficher le menu Bluetooth
void displayBluetooth() {
  currentPage = PAGE_BLUETOOTH;
  tft.fillScreen(TFT_BLACK); // Effacer l'écran

  // Vérifier l'état de la connexion Bluetooth
  checkBluetoothConnection();

  // Afficher l'état de la connexion Bluetooth
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(1);
  tft.setCursor(SIDEBAR_WIDTH + 10, 30);
  if (deviceConnected) {
    tft.println("Connecté");
  } else {
    tft.println("Non Connecté");
  }

  // Dessiner les boutons
  drawButton(tft, btnPrevious);
  drawButton(tft, btnPlayPause);
  drawButton(tft, btnNext);
}

// Fonction pour gérer les appuis tactiles sur les boutons
void checkBluetoothTouch(uint16_t t_x, uint16_t t_y) {
  // Vérifier si un bouton a été pressé et envoyer les commandes Bluetooth correspondantes
  if (isButtonPressed(btnPrevious, t_x, t_y)) {
    sendPreviousCommand();  // Envoyer la commande Précédent
    Serial.println("Bouton 'Précédent' pressé");
  } else if (isButtonPressed(btnPlayPause, t_x, t_y)) {
    sendPlayPauseCommand();  // Envoyer la commande Play/Pause
    Serial.println("Bouton 'Play/Pause' pressé");
  } else if (isButtonPressed(btnNext, t_x, t_y)) {
    sendNextCommand();  // Envoyer la commande Suivant
    Serial.println("Bouton 'Suivant' pressé");
  }
}

#endif
