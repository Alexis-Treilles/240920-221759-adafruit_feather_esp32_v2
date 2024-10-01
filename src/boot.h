#ifndef BOOT_H
#define BOOT_H

#include "TFT_eSPI.h"  // Inclure la bibliothèque TFT_eSPI
#include "globals.h"      // Inclure les variables globales
#include "home.h"         // Inclure le menu principal
#include "menu_fonctionnement.h"   // Inclure le menu Fonctionnement
#include "menu_bluetooth.h"    // Inclure le menu Bluetooth
#include "menu_gps.h"   // Inclure le menu GPS
#include "menu_timer.h" // Inclure le menu Timer
#include "entete.h"  // Inclure l'entête
#include "temp.h"    // Inclure le fichier pour la mise à jour des données de température
#include "gps.h"    // Inclure le fichier pour la mise à jour des données GPS
#include <TinyGPS++.h>     // Bibliothèque GPS
#include "log.h"           // Inclure le fichier log pour la journalisation
#include "intarissables_image.h"  // Inclure le fichier de l'image
#include "button.h"  // Inclure les boutons pour l'interaction utilisateur

// Boutons pour la sélection du conducteur
Button btnAlexis(40, 100, 120, 60, "Alexis", TFT_BLUE);  // Bouton pour Alexis
Button btnRobin(180, 100, 120, 60, "Robin", TFT_BLUE);   // Bouton pour Robin

bool driverSelected = false;  // Variable pour savoir si le conducteur a été sélectionné
String driverName = "Alexis";  // Stocker le nom du conducteur sélectionné

// Fonction pour afficher l'image bicolore stockée dans la variable intarissables_image
void displayImage() {
  int width = 320;   // Largeur de l'image
  int height = 240;  // Hauteur de l'image
  int byteWidth = (width + 7) / 8;  // Nombre d'octets par ligne (8 pixels par octet)

  // Parcourir chaque ligne et chaque pixel pour afficher l'image
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      // Calculer la position du pixel dans le tableau
      int byteIndex = (y * byteWidth) + (x / 8);
      int bitIndex = x % 8;
      
      // Extraire le pixel (0 pour blanc, 1 pour bleu)
      bool isBlue = !(intarissables_image[byteIndex] & (1 << (7 - bitIndex)));
      
      // Afficher le pixel en fonction de la couleur
      if (isBlue) {
        tft.drawPixel(x, y, TFT_BLUE);
      } else {
        tft.drawPixel(x, y, TFT_WHITE);
      }
    }
  }
}

// Fonction pour afficher le menu "Qui conduit ?"
void displayDriverSelection() {
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);
  tft.setCursor(60, 40);
  tft.println("Qui conduit ?");

  // Dessiner les boutons
  drawButton(tft, btnAlexis);
  drawButton(tft, btnRobin);
}

// Fonction pour vérifier si un bouton est pressé dans la sélection du conducteur
void checkDriverSelection(uint16_t t_x, uint16_t t_y) {
  if (isButtonPressed(btnAlexis, t_x, t_y)) {
    Serial.println("Alexis conduit.");
    driverName = "Alexis";  // Stocker Alexis comme conducteur
    driverSelected = true;
  } else if (isButtonPressed(btnRobin, t_x, t_y)) {
    Serial.println("Robin conduit.");
    driverName = "Robin";  // Stocker Robin comme conducteur
    driverSelected = true;
  }
}

// Fonction de démarrage principale qui appelle les initialisations
void bootSetup() {
  tft.fillScreen(TFT_WHITE);  // Remplir l'écran avec la couleur blanche
  tft.setFreeFont(&FreeSerif9pt7b);  // Charger une police plus petite

  // Afficher l'image de démarrage
  displayImage();
  delay(3000);  // Attendre 3 secondes avant de continuer

  // Appels des fonctions d'initialisation avec des impressions de débogage
  Serial.println("Initialisation du GPS...");
  initGPS();  // Initialiser le GPS
  delay(1000);
  Serial.println("Initialisation du Bluetooth...");
  initBluetooth();  // Initialiser le Bluetooth
  delay(1000);
  Serial.println("Initialisation de la carte SD...");
  initSDCard();  // Initialiser la carte SD
  delay(1000);
  Serial.println("Initialisation du capteur de température...");
  sensors.begin();  // Initialiser le capteur DS18B20
  tft.setCursor(240, 220);
  tft.println("TMP");
  delay(1000);

  // Afficher le menu "Qui conduit ?"
  displayDriverSelection();

  // Attendre que l'utilisateur sélectionne un conducteur, sinon passer au menu home
  unsigned long startTime = millis();
  while (!driverSelected) {
    if (millis() - startTime > 10000) {
      // Si aucun conducteur n'est sélectionné après 10 secondes, afficher le menu home
      Serial.println("Aucun conducteur sélectionné après 10 secondes. Passage au menu home.");
      displayHome();
      return;
    }

    // Vérifier si un toucher tactile a eu lieu pour sélectionner le conducteur
    uint16_t t_x, t_y;
    if (tft.getTouch(&t_x, &t_y)) {
      checkDriverSelection(t_x, t_y);
    }
  }

  // Afficher le menu home après sélection
  Serial.print(driverName);
  Serial.println(" est sélectionné comme conducteur.");
  displayHome();  // Afficher le menu home après la sélection du conducteur
}

#endif  // BOOT_H
