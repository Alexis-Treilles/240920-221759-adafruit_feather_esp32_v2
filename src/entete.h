#ifndef ENTETE_H
#define ENTETE_H

#include <TFT_eSPI.h>
#include "globals.h"
#include "button.h"  // Inclure le fichier contenant la structure Button et les fonctions associées

extern void displayHome(); // Déclaration de la fonction displayHome
extern TFT_eSPI tft;
extern bool sdAvailable;

// Définir l'état du clignotement du cercle
bool isOrange = false;  // État de la couleur du cercle

// Dimensions de l'entête verticale sur le côté gauche
#define ENTETE_WIDTH 60
#define ENTETE_HEIGHT tft.height()

// Coordonnées et dimensions du bouton "Menu Principal"
#define MENU_BUTTON_W ENTETE_WIDTH
#define MENU_BUTTON_H 60

// Définition du bouton "Menu Principal"
Button btnMenuPrincipal(5, 5, MENU_BUTTON_W - 10, MENU_BUTTON_H - 10, "Menu");

// Variables globales pour stocker les anciennes valeurs afin d'éviter de redessiner inutilement l'entête
String lastTime = "";
int lastSatellites = -1;
int lastTemperature = -1;
bool lastDeviceConnected = false;
bool lastSdAvailable = false; // Variable pour l'état de la carte SD
Page lastPage = currentPage; // Stocker la dernière page affichée


int circleX = 25; // Position X du cercle (à ajuster selon votre design)
int circleY = 80; // Position Y du cercle (sous le bouton Menu Principal)
// Fonction pour dessiner l'entête
void drawEntete() {
  // Vérifier si une des variables a changé ou si la page a changé
  if (currentTime != lastTime || currentSatellites != lastSatellites ||
      currentTemperature != lastTemperature || deviceConnected != lastDeviceConnected ||
      sdAvailable != lastSdAvailable || currentPage != lastPage) {

    // Mettre à jour les anciennes valeurs
    lastTime = currentTime;
    lastSatellites = currentSatellites;
    lastTemperature = currentTemperature;
    lastDeviceConnected = deviceConnected;
    lastSdAvailable = sdAvailable; // Mettre à jour l'état de la carte SD
    lastPage = currentPage; // Mettre à jour la dernière page

    // Dessiner l'entête verticale sur la gauche
    tft.fillRect(0, 0, ENTETE_WIDTH, ENTETE_HEIGHT, TFT_DARKGREY); // Fond gris foncé
    tft.setTextSize(0);
    tft.setTextColor(TFT_WHITE);

    // Dessiner le bouton "Menu Principal"
    drawButton(tft, btnMenuPrincipal);

    // Affichage de l'heure au milieu
    tft.setCursor(4, ENTETE_HEIGHT / 2 - 3); // Centré verticalement
    tft.println(currentTime.substring(0, 5));  // Affiche uniquement les 5 premiers caractères

    // Affichage de la température sous l'heure
    tft.setCursor(5, 2 * ENTETE_HEIGHT / 3); // Sous l'heure avec un petit écart
    tft.println(String(currentTemperature) + "°C");

    // Nombre de satellites (remonté un peu plus haut)
    tft.setCursor(5, ENTETE_HEIGHT - 50); 
    tft.println(String(currentSatellites) + " Sat");

    // Statut Bluetooth à droite des satellites
    if (deviceConnected) {
      tft.fillCircle(ENTETE_WIDTH - 15, ENTETE_HEIGHT - 25, 10, TFT_GREEN);
    } else {
      tft.fillCircle(ENTETE_WIDTH - 15, ENTETE_HEIGHT - 25, 10, TFT_RED);
    }

    // Indicateur de disponibilité de la carte SD à gauche du Bluetooth
    if (sdAvailable) {
      tft.fillCircle(ENTETE_WIDTH - 45, ENTETE_HEIGHT - 25, 10, TFT_GREEN); // SD disponible
    } else {
      tft.fillCircle(ENTETE_WIDTH - 45, ENTETE_HEIGHT - 25, 10, TFT_RED); // SD non disponible
    }

    
  }
  // Dessiner le cercle clignotant sous le bouton Menu Principal
    if (isOrange) {
      tft.fillCircle(circleX, circleY, 10, TFT_ORANGE); // Cercle orange
    } else {
      tft.fillCircle(circleX, circleY, 10, TFT_YELLOW); // Cercle jaune
    }
    isOrange = !isOrange; // Alterner l'état de la couleur
    //Serial.println(isOrange);
}

// Fonction pour gérer les appuis tactiles dans l'entête
void checkEnteteTouch(uint16_t t_x, uint16_t t_y) {
  // Vérifier si l'utilisateur a appuyé sur le bouton "Menu Principal"
  if (isButtonPressed(btnMenuPrincipal, t_x, t_y)) {
    displayHome(); // Rediriger vers la page "home"
  }
}

#endif
