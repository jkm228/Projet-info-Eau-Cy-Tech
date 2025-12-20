# Projet-info-Eau-Cy-Tech

Projet C et Shell du groupe **MEF2-B**.
**Auteurs :** Joseph Kouto-messan, Milo Thomas et Adam Arezki.

 # Description
C-WildWater est une application de traitement et d'analyse de données massives (Big Data) pour les réseaux de distribution d'eau.

Le projet combine la puissance du Shell Script (pour le filtrage et le nettoyage des données) et la performance du Langage C (pour le stockage et les calculs complexes).

L'application permet de :

Analyser les flux : Calculer les volumes d'eau (capacité, consommation réelle) des stations.

Détecter les fuites : Identifier les pertes d'eau dans le réseau en aval d'une usine spécifique.

Visualiser les résultats : Générer automatiquement des graphiques histogrammes via Gnuplot.

# Installation et Compilation

# 1. Cloner le dépôt 
git clone https://github.com/jkm228/Projet-info-Eau-Cy-Tech.git

# 2. Rentrer dans le dossier
cd Projet-info-Eau-Cy-Tech

# 3. Vérifier que les fichiers sont bien là
ls -l

# 4. Rendre le script exécutable
chmod +x c-wildwater.sh

# 5. Compiler le programme C
make

# 6. Lancer le calcul max
./c-wildwater.sh c-wildwater_v3.dat histo max

# 7. Ouvrir l'image max
xdg-open vol_max.png

# 6. Lancer le calcul src
./c-wildwater.sh c-wildwater_v3.dat histo src

# 7. Ouvrir l'image src
xdg-open vol_captation.png

# 10. Lancer le calcul real
./c-wildwater.sh c-wildwater_v3.dat histo real

# 11. Ouvrir l'image real
xdg-open vol_traitement.png

# 12. Lancer le calcul pour une usine (exemple : Plant #VC300055Q)
./c-wildwater.sh c-wildwater_v3.dat leaks "Plant #VC300055Q"

# 13. Afficher le résultat dans le terminal
cat leaks.csv

# Afficher le début des fichiers csv triée de max,src et real
head -n 20 vol_max.dat
head -n 20 vol_captation.dat
head -n 20 vol_traitement.dat

# Organisation du Code
Le programme mixe l'utilisation de Shell et C :

c-wildwater.sh 

Vérifie les arguments et les fichiers.

Filtre le fichier CSV brut avec awk (suppression des colonnes inutiles, formatage).

Gère la compatibilité numérique (points vs virgules pour un bon fonctionnement sur linux).

Pilote Gnuplot pour la génération des images.

# Le  C (Calcul) :
main.c : Logique principale et gestion des arguments.

file.c : Lecture du fichier temporaire et chargement en mémoire.

avl.c / avl.h : Implémentation d'un Arbre AVL (Arbre Binaire de Recherche Équilibré) pour stocker les stations pour une recherche rapide.

Modélisation Graphe : Chaque station possède une liste chaînée de ses connexions aval (liste_aval), permettant un parcours récursif du réseau.

# Remarque importante :
 Les volumes d'eau traités par les stations étant très importants (souvent supérieurs à plusieurs millions de mètres cubes), l'utilisation de l'unité brute ($m^3$) rendait l'axe des ordonnées du graphique illisible.Nous avons donc fait le choix de convertir les données en Millions de mètres cubes ($M.m^3$) dès la phase de pré-traitement des données.Cette opération est effectuée dans le script Shell via la commande awk, en divisant les volumes bruts par 1 000 000. Cela garantit que l'affichage graphique est à la fois lisible et cohérent avec l'étiquette de l'axe Y.

Pour les histogrammes des top 50 plus petites usines les noms des usines ne sont pas affichés en abscisses car sinon l'affichage n'est plus visible. Mais si on ouvre les fichiers csv générés des modes max,src et real, les valeurs en début les plus à gauche du graphique ( les plus petites ) sont bien les valeurs les plus en bas du fichier csv correspondant ( trie décroissant donc les plus grands en haut et les plus petits en bas ).
