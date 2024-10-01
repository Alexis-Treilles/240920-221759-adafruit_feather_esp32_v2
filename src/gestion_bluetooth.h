#ifndef GESTION_BLUETOOTH_H
#define GESTION_BLUETOOTH_H

#include <BLEDevice.h>
#include <BLEHIDDevice.h>
#include <BLEServer.h>
#include "globals.h"

// Définitions des codes de touches multimédia
#define HID_CONSUMER_PLAY_PAUSE          0x00CD
#define HID_CONSUMER_SCAN_PREVIOUS_TRACK 0x00B6
#define HID_CONSUMER_SCAN_NEXT_TRACK     0x00B5

bool previousDeviceConnected = false;
BLEHIDDevice* hid;
BLECharacteristic* input;
unsigned long lastDisconnectTime = 0;  // Pour enregistrer le temps de la dernière déconnexion
unsigned long reconnectDelay = 5000;   // Délai de 5 secondes avant de relancer la publicité

// Fonction pour vérifier la connexion Bluetooth
void checkBluetoothConnection() {
  if (deviceConnected != previousDeviceConnected) {
    if (deviceConnected) {
      Serial.println("Périphérique connecté");
    } else {
      Serial.println("Périphérique déconnecté");
      lastDisconnectTime = millis();  // Enregistrer le temps de la déconnexion
    }
    previousDeviceConnected = deviceConnected;
  }

  // Si le périphérique est déconnecté et que le délai est écoulé
  if (!deviceConnected && millis() - lastDisconnectTime >= reconnectDelay) {
    BLEDevice::startAdvertising();  // Recommencer la publicité après le délai
    Serial.println("Publicité relancée après déconnexion");
  }
}

class MyCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    deviceConnected = true;
    pServer->getAdvertising()->stop();  // Arrêter la publicité une fois connecté
    Serial.println("Périphérique connecté automatiquement après appairage");
  }

  void onDisconnect(BLEServer* pServer) {
    deviceConnected = false;
    Serial.println("Périphérique déconnecté");
    lastDisconnectTime = millis();  // Enregistrer le temps de la déconnexion pour la reconnexion
  }
};

// Fonction pour initialiser le Bluetooth HID
void initBluetooth() {
  Serial.println("Initialisation du Bluetooth HID...");

  // Initialiser le périphérique BLE
  BLEDevice::init("ESP32 Media Remote");
  BLEServer* pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyCallbacks());

  hid = new BLEHIDDevice(pServer);
  input = hid->inputReport(1);  // Report ID 1

  hid->manufacturer()->setValue("ESP32");

  // Définir le rapport HID pour les commandes multimédia
  const uint8_t reportMap[] = {
    0x05, 0x0C,                    // Usage Page (Consumer Devices)
    0x09, 0x01,                    // Usage (Consumer Control)
    0xA1, 0x01,                    // Collection (Application)
    0x85, 0x01,                    // Report Id (1)
    0x15, 0x00,                    // Logical Minimum (0)
    0x26, 0xFF, 0x03,              // Logical Maximum (0x03FF)
    0x19, 0x00,                    // Usage Minimum (0)
    0x2A, 0xFF, 0x03,              // Usage Maximum (0x03FF)
    0x75, 0x10,                    // Report Size (16 bits)
    0x95, 0x01,                    // Report Count (1)
    0x81, 0x00,                    // Input (Data, Array)
    0xC0                           // End Collection
  };

  hid->reportMap((uint8_t*)reportMap, sizeof(reportMap));

  // Démarrer les services HID après un léger délai (utilisation de millis() pour éviter delay())
  unsigned long startTime = millis();
  while (millis() - startTime < 1000) {
    // Attendre 1 seconde pour garantir une bonne initialisation (non bloquant)
  }
  hid->startServices();  // Démarrer les services HID après un délai

  // Configurer la sécurité BLE avec le mode de bonding
  BLESecurity *pSecurity = new BLESecurity();
  pSecurity->setAuthenticationMode(ESP_LE_AUTH_BOND);  // Activer uniquement le bonding
  pSecurity->setCapability(ESP_IO_CAP_NONE);  // Pas de capacité d'entrée (pas de PIN)
  pSecurity->setInitEncryptionKey(ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK);

  // Démarrer la publicité BLE avec un intervalle plus stable
  BLEAdvertising* pAdvertising = pServer->getAdvertising();
  pAdvertising->setMinInterval(0x50);  // Intervalle minimal (80 unités de 0.625 ms = 50 ms)
  pAdvertising->setMaxInterval(0x100); // Intervalle maximal (256 unités de 0.625 ms = 160 ms)
  pAdvertising->start();

  Serial.println("Bluetooth HID initialisé avec sécurité et en attente de connexion...");
  tft.setCursor(120,220);
  tft.println("BT");
}

// Fonction pour envoyer une commande multimédia (Play/Pause, Suivant, Précédent)
void sendMediaKey(uint16_t key) {
  Serial.print("Envoi de la commande multimédia : 0x");
  Serial.println(key, HEX);

  uint8_t report[2];
  report[0] = key & 0xFF;
  report[1] = (key >> 8) & 0xFF;

  // Envoi de la commande HID
  input->setValue(report, sizeof(report));
  input->notify();

  Serial.print("Rapport HID envoyé : ");
  Serial.print("0x");
  Serial.print(report[1], HEX);
  Serial.print(" 0x");
  Serial.println(report[0], HEX);

  delay(100);

  // Relâcher le bouton
  report[0] = 0x00;
  report[1] = 0x00;
  input->setValue(report, sizeof(report));
  input->notify();

  Serial.println("Commande relâchée (rapport HID 0x00 0x00 envoyé)");
}

// Fonction pour envoyer la commande Play/Pause
void sendPlayPauseCommand() {
  Serial.println("Envoi de la commande Play/Pause...");
  sendMediaKey(HID_CONSUMER_PLAY_PAUSE);
}

// Fonction pour envoyer la commande Suivant
void sendNextCommand() {
  Serial.println("Envoi de la commande Suivant...");
  sendMediaKey(HID_CONSUMER_SCAN_NEXT_TRACK);
}

// Fonction pour envoyer la commande Précédent
void sendPreviousCommand() {
  Serial.println("Envoi de la commande Précédent...");
  sendMediaKey(HID_CONSUMER_SCAN_PREVIOUS_TRACK);
}

#endif
