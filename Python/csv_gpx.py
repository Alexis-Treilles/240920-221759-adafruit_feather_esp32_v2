import os
import pandas as pd
from xml.etree.ElementTree import Element, SubElement, ElementTree
import datetime

# Obtenir le répertoire où se trouve le fichier de script
script_dir = os.path.dirname(os.path.realpath(__file__))

# Construire les chemins pour les dossiers csv et gpx basés sur le répertoire du script
csv_dir = os.path.join(script_dir, 'csv/')
gpx_dir = os.path.join(script_dir, 'gpx/')

# S'assurer que le répertoire de sortie existe
if not os.path.exists(gpx_dir):
    os.makedirs(gpx_dir)

# Fonction pour convertir un fichier CSV en fichier GPX
def csv_to_gpx(csv_file, gpx_file):
    # Lire le CSV
    data = pd.read_csv(csv_file)
    
    # Créer l'élément racine du fichier GPX
    gpx = Element('gpx', version="1.1", creator="Custom GPX Generator", xmlns="http://www.topografix.com/GPX/1/1")
    
    # Ajouter les métadonnées
    metadata = SubElement(gpx, 'metadata')
    time = SubElement(metadata, 'time')
    time.text = datetime.datetime.utcnow().isoformat() + "Z"
    
    # Ajouter les éléments track et track segment
    trk = SubElement(gpx, 'trk')
    trkseg = SubElement(trk, 'trkseg')
    
    # Boucle pour chaque ligne du CSV
    for index, row in data.iterrows():
        trkpt = SubElement(trkseg, 'trkpt', lat=str(row[2]), lon=str(row[3]))  # Latitude et Longitude
        ele = SubElement(trkpt, 'ele')
        ele.text = str(row[4])  # Altitude
        speed = SubElement(trkpt, 'speed')
        speed.text = str(row[5])  # Vitesse
        time = SubElement(trkpt, 'time')
        # Combiner la date et l'heure et convertir en format ISO
        dt_str = f"{row[0]} {row[1]}"
        dt = datetime.datetime.strptime(dt_str, "%d/%m/%Y %H:%M:%S")
        time.text = dt.isoformat() + "Z"
    
    # Écrire les données GPX dans le fichier
    tree = ElementTree(gpx)
    tree.write(gpx_file, encoding='utf-8', xml_declaration=True)

# Parcourir tous les fichiers CSV dans le répertoire source
for file_name in os.listdir(csv_dir):
    if file_name.endswith('.csv'):
        # Chemin complet du fichier CSV
        csv_file_path = os.path.join(csv_dir, file_name)
        
        # Chemin complet du fichier GPX à créer
        gpx_file_name = os.path.splitext(file_name)[0] + '.gpx'
        gpx_file_path = os.path.join(gpx_dir, gpx_file_name)
        
        # Convertir le fichier CSV en GPX
        csv_to_gpx(csv_file_path, gpx_file_path)

print("Conversion terminée.")
