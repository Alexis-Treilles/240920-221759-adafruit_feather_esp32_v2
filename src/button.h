#ifndef BUTTON_H
#define BUTTON_H

#include <TFT_eSPI.h>

// Structure pour définir un bouton
struct Button {
  uint16_t x, y, w, h;
  const char* label;
  bool isPressed;
  unsigned long lastPressed; // Dernière fois que le bouton a été pressé
  uint16_t color;  // Couleur du bouton

  // Constructeur avec couleur par défaut (TFT_BLUE)
  Button(uint16_t _x, uint16_t _y, uint16_t _w, uint16_t _h, const char* _label, uint16_t _color = TFT_BLUE)
      : x(_x), y(_y), w(_w), h(_h), label(_label), isPressed(false), lastPressed(0), color(_color) {}
};

// Fonction pour dessiner un bouton arrondi avec texte centré
void drawButton(TFT_eSPI &tft, Button &btn) {
  uint16_t radius = 10;  // Rayon des coins arrondis

  // Dessiner le bouton avec des coins arrondis
  tft.fillRoundRect(btn.x, btn.y, btn.w, btn.h, radius, btn.color);  // Rectangle avec coins arrondis
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(1);

  // Calculer la largeur du texte
  int16_t textWidth = tft.textWidth(btn.label);

  // Calculer la hauteur du texte (pour une taille 1, c'est environ 8 pixels)
  uint16_t textHeight =12;  // Hauteur approximative du texte pour la taille 1

  // Calculer les coordonnées pour centrer le texte
  uint16_t text_x = btn.x + (btn.w - textWidth) / 2;
  uint16_t text_y = btn.y + (btn.h + textHeight) / 2;

  // Dessiner le texte centré
  tft.setCursor(text_x, text_y);
  tft.println(btn.label);
}

// Fonction pour vérifier si un bouton est pressé avec anti-rebond (debounce)
bool isButtonPressed(Button &btn, uint16_t t_x, uint16_t t_y, unsigned long debounceDelay = 400) {
  unsigned long currentMillis = millis();  // Temps actuel

  if (t_x >= btn.x && t_x < (btn.x + btn.w) && t_y >= btn.y && t_y < (btn.y + btn.h)) {
    // Vérifier si suffisamment de temps s'est écoulé depuis la dernière pression
    if (currentMillis - btn.lastPressed >= debounceDelay) {
      btn.isPressed = true;
      btn.lastPressed = currentMillis;  // Mettre à jour le dernier temps de pression
      return true;
    }
  }

  btn.isPressed = false;
  return false;
}

#endif
