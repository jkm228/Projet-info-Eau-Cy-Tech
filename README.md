# Projet-info-Eau-Cy-Tech

Projet C et Shell du groupe **MEF2-B**.
**Auteurs :** Joseph Kouto-messan, Milo Thomas et Adam Arezki.

## 🚀 Installation

# 1. Cloner le projet github
git clone https://github.com/jkm228/Projet-info-Eau-Cy-Tech.git

# 2. Entrer dans le dossier du projet
cd Projet-info-Eau-Cy-Tech

# 3. Mettre les permissions de l'exécutable
chmod +x c-wildwater.sh

# 4. Compiler le projet
make

# 5. Exécution 
./c-wildwater.sh c-wildwater_v3.dat histo max: pour l'histogramme montrant quantité d’eau que l’usine peut traiter par an. 
./c-wildwater.sh c-wildwater_v3.dat histo real: pour l'histogramme montrant la somme des volumes d’eau prelevés par an par toutes les sources qui alimentent une usine
./c-wildwater.sh c-wildwater_v3.dat histo src: pour l'histogramme montrant la somme pondérée des volumes captés par les sources multipliés par le pourcentage d’eau qui parvient aux usines
