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
#include "gestion_timer.h" // Inclure la gestion des timers (ajout)


// Boutons pour la sélection du conducteur
Button btnAlexis(40, 100, 120, 60, "Alexis", TFT_BLUE);  // Bouton pour Alexis
Button btnRobin(180, 100, 120, 60, "Robin", TFT_BLUE);   // Bouton pour Robin

bool driverSelected = false;  // Variable pour savoir si le conducteur a été sélectionné
String driverName = "Alexis";  // Stocker le nom du conducteur sélectionné

// Fonction pour initialiser l'écran et le tactile, y compris la calibration
void initScreenAndTouch() {
    Serial.println("Initialisation de l'écran et du tactile...");

    tft.begin();            // Initialiser l'écran TFT
    tft.setRotation(3);     // Orientation de l'écran

    // Initialiser SPIFFS pour vérifier et stocker la calibration
    if (!SPIFFS.begin()) {
        Serial.println("Erreur d'initialisation de SPIFFS !");
        return;  // Si SPIFFS ne peut pas être initialisé, on arrête ici
    }

    // Vérifier si le fichier de calibration existe déjà
    if (SPIFFS.exists("/TouchCalData")) {
        // Charger les données de calibration
        File f = SPIFFS.open("/TouchCalData", "r");
        if (f) {
            uint16_t calData[5];
            for (int i = 0; i < 5; i++) {
                calData[i] = f.readStringUntil('\n').toInt();
            }
            tft.setTouch(calData);  // Appliquer la calibration
            Serial.println("Données de calibration tactiles chargées.");
        } else {
            Serial.println("Erreur lors de la lecture des données de calibration !");
        }
        f.close();
    } else {
        // Si aucune calibration n'existe, faire une nouvelle calibration
        Serial.println("Aucune donnée de calibration trouvée, exécution de la calibration...");
        uint16_t calData[5];
        tft.calibrateTouch(calData, TFT_WHITE, TFT_BLACK, 15);  // Exécuter la calibration

        // Sauvegarder les données de calibration
        File f = SPIFFS.open("/TouchCalData", "w");
        if (f) {
            for (int i = 0; i < 5; i++) {
                f.println(calData[i]);
            }
            f.close();
            Serial.println("Données de calibration tactiles enregistrées.");
        } else {
            Serial.println("Erreur lors de l'enregistrement des données de calibration !");
        }
    }

    Serial.println("Initialisation de l'écran et du tactile terminée.");
}

// Fonction pour afficher l'image bicolore stockée dans la variable intarissables_image
void displayImage() {
    int width = 320;   // Largeur de l'image
    int height = 240;  // Hauteur de l'image
    int byteWidth = (width + 7) / 8;  // Nombre d'octets par ligne (8 pixels par octet)

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int byteIndex = (y * byteWidth) + (x / 8);
            int bitIndex = x % 8;
            
            bool isBlue = !(intarissables_image[byteIndex] & (1 << (7 - bitIndex)));
            
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

    drawButton(tft, btnAlexis);
    drawButton(tft, btnRobin);
}

// Fonction pour vérifier si un bouton est pressé dans la sélection du conducteur
void checkDriverSelection(uint16_t t_x, uint16_t t_y) {
    if (isButtonPressed(btnAlexis, t_x, t_y)) {
        Serial.println("Alexis conduit.");
        driverName = "Alexis";
        driverSelected = true;
    } else if (isButtonPressed(btnRobin, t_x, t_y)) {
        Serial.println("Robin conduit.");
        driverName = "Robin";
        driverSelected = true;
    }
}

// Fonction de démarrage principale qui appelle les initialisations
void bootSetup() {
    tft.fillScreen(TFT_WHITE);  
    tft.setFreeFont(&FreeSerif9pt7b);

    displayImage();
    Serial.println("Initialisation des composants...");
    initGPS();
    delay(500);
    initBluetooth();
    delay(500);
    initSDCard();
    delay(500);
    initTimers();          // Initialiser les timers à partir du fichier CSV ou initialiser à 0

    sensors.begin();
    tft.setCursor(240, 220);
    tft.println("TMP");
    delay(500);

    displayDriverSelection();

    unsigned long startTime = millis();
    while (!driverSelected) {
        if (millis() - startTime > 10000) {
            Serial.println("Aucun conducteur sélectionné après 10 secondes. Passage au menu home.");
            displayHome();
            return;
        }

        uint16_t t_x, t_y;
        if (tft.getTouch(&t_x, &t_y)) {
            t_x = TFT_WIDTH - t_x; 
            checkDriverSelection(t_x, t_y);
        }
    }

    Serial.print(driverName);
    Serial.println(" est sélectionné comme conducteur.");
    displayHome();
}

#endif  // BOOT_H
