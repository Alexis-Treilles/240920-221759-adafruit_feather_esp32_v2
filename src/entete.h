#ifndef ENTETE_H
#define ENTETE_H

#include <TFT_eSPI.h>
#include "globals.h"
#include "button.h"  // Inclure le fichier contenant la structure Button et les fonctions associées

extern void displayHome(); // Déclaration de la fonction displayHome
extern TFT_eSPI tft;

// Dimensions de l'entête verticale sur le côté gauche
#define ENTETE_WIDTH 60
#define ENTETE_HEIGHT tft.height()

// Coordonnées et dimensions du bouton "Menu Principal"
#define MENU_BUTTON_W ENTETE_WIDTH
#define MENU_BUTTON_H 60

// Définition du bouton "Menu Principal"
Button btnMenuPrincipal(5, 5, MENU_BUTTON_W - 10, MENU_BUTTON_H - 10, "Menu");

// Fonction pour dessiner l'entête
void drawEntete() {
  // Variables statiques pour stocker les anciennes valeurs
  static String lastTime = "";
  static int lastSatellites = -1;
  static int lastTemperature = -1;
  static bool lastDeviceConnected = false;
  static Page lastPage = currentPage; // Stocker la dernière page affichée

  // Vérifier si une des variables a changé ou si la page a changé
  if (currentTime != lastTime || currentSatellites != lastSatellites ||
      currentTemperature != lastTemperature || deviceConnected != lastDeviceConnected ||
      currentPage != lastPage) {

    // Imprimer la date et le nombre de satellites dans la console
    Serial.print("Date actuelle : ");
    Serial.println(currentTime);  // Affiche l'heure ou la date
    Serial.print("Nombre de satellites : ");
    Serial.println(currentSatellites);

    // Mettre à jour les anciennes valeurs
    lastTime = currentTime;
    lastSatellites = currentSatellites;
    lastTemperature = currentTemperature;
    lastDeviceConnected = deviceConnected;
    lastPage = currentPage; // Mettre à jour la dernière page

    // Dessiner l'entête verticale sur la gauche
    tft.fillRect(0, 0, ENTETE_WIDTH, ENTETE_HEIGHT, TFT_DARKGREY); // Fond gris foncé
    tft.setTextSize(0);
    tft.setTextColor(TFT_WHITE);

    // Dessiner le bouton "Menu Principal"
    drawButton(tft, btnMenuPrincipal);

    // Affichage de l'heure au milieu
    tft.setCursor(1, ENTETE_HEIGHT / 2 - 3); // Centré verticalement
    tft.println(currentTime);

    // Affichage de la température sous l'heure
    tft.setCursor(5, 2 * ENTETE_HEIGHT / 3); // Sous l'heure avec un petit écart
    tft.println(String(currentTemperature) + "°C");

    // Nombre de satellites en bas à gauche
    tft.setCursor(5, ENTETE_HEIGHT - 32); 
    tft.println(String(currentSatellites));

    // Statut Bluetooth à droite des satellites
    if (deviceConnected) {
      tft.fillCircle(ENTETE_WIDTH - 15, ENTETE_HEIGHT - 25, 10, TFT_GREEN);
    } else {
      tft.fillCircle(ENTETE_WIDTH - 15, ENTETE_HEIGHT - 25, 10, TFT_RED);
    }
  }
}

// Fonction pour gérer les appuis tactiles dans l'entête
void checkEnteteTouch(uint16_t t_x, uint16_t t_y) {
  // Vérifier si l'utilisateur a appuyé sur le bouton "Menu Principal"
  if (isButtonPressed(btnMenuPrincipal, t_x, t_y)) {
    displayHome(); // Rediriger vers la page "home"
  }
}

#endif
