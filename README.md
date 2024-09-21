
# Projet 4L_V4

Ce dépôt contient le code source pour le projet **4L_V4**, un projet basé sur Arduino qui intègre diverses fonctionnalités telles que le GPS, la gestion Bluetooth, des timers, et plus encore.

## Table des matières

1. [Description du projet](#description-du-projet)
2. [Structure du projet](#structure-du-projet)
3. [Installation](#installation)
4. [Utilisation](#utilisation)
5. [Fonctionnalités](#fonctionnalités)
6. [Contributions](#contributions)
7. [Licence](#licence)

## Description du projet

Le projet **4L_V4** est une application embarquée développée pour une plateforme Arduino. Il combine plusieurs modules et fonctionnalités, notamment :
- Gestion du GPS pour le suivi de localisation
- Bluetooth pour la communication sans fil
- Timers et gestion des événements
- Menus pour l’interface utilisateur

## Structure du projet

Voici une description des principaux fichiers et répertoires du projet :
- `4L_V4.ino` : Fichier principal de l'application Arduino.
- `menu_debug.h` : Gère le menu de débogage pour l'interface utilisateur.
- `menu_bluetooth.h` : Gère la fonctionnalité Bluetooth, permettant la configuration et la gestion des connexions.
- `menu_fonctionnement.h` : Contient la logique pour les fonctionnalités liées au fonctionnement général du système.
- `menu_timer.h` : Gère les timers et la gestion des événements basés sur le temps.
- `menu_gps.h` : Contient la gestion du module GPS pour le suivi de la position.
- `gps.h` : Fichier contenant les fonctions de gestion et d'intégration avec le module GPS.
- `temp.h` : Fichier temporaire ou contenant des données relatives à la gestion de la température.
- `globals.h` : Fichier où sont stockées les variables globales et les constantes.
- `home.h` : Fichier d'interface pour la page d'accueil du menu.
- `gestion_bluetooth.h` : Gère la connexion Bluetooth et les événements associés.
- `button.h` : Gestion des boutons physiques de l'interface utilisateur.
- `menu_features.h` : Fichier contenant les différentes fonctionnalités du menu.
- `entete.h` : Fichier d'en-tête global pour l'application.

## Installation

1. Clonez le dépôt :
   ```bash
   git clone https://github.com/votre-utilisateur/4L_V4.git
   ```
2. Ouvrez le fichier `4L_V4.ino` dans l'IDE Arduino.

3. Téléchargez les dépendances nécessaires pour les modules Bluetooth, GPS, etc. via le gestionnaire de bibliothèques Arduino.

4. Compilez et téléversez le code sur votre carte Arduino.

## Utilisation

Une fois le code téléversé, vous pouvez interagir avec le projet via les options suivantes :
- **Menu GPS** : Affiche la localisation actuelle à l'aide du module GPS.
- **Menu Bluetooth** : Permet de gérer les connexions Bluetooth.
- **Timers** : Configurez des événements basés sur le temps.
- **Débogage** : Utilisez le menu de débogage pour visualiser les logs et tester les fonctionnalités.

## Fonctionnalités

- **GPS** : Suivi en temps réel de la position via GPS.
- **Bluetooth** : Communication sans fil avec d'autres appareils.
- **Timers** : Gérer les événements selon une base temporelle.
- **Interface utilisateur** : Système de menu interactif pour gérer les différentes fonctionnalités.

## Contributions

Les contributions sont les bienvenues ! Si vous avez des idées d'amélioration ou si vous rencontrez des bugs, veuillez ouvrir une issue ou soumettre une pull request.

## Licence

Ce projet est sous licence MIT. Voir le fichier [LICENSE](LICENSE) pour plus de détails.
