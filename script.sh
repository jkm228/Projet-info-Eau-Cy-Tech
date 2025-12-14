#!/bin/bash

# --- VÉRIFICATIONS PRÉLIMINAIRES ---
if [ $# -lt 2 ]; then
    echo "Erreur : Arguments manquants."
    echo "Usage: $0 <fichier_dat> <station_type> [consumer_type]"
    echo "Exemple: $0 c-wildwater_v3.dat histo max"
    exit 1
fi

FICHIER_DAT="$1"
COMMANDE="$2"
OPTION="$3"
EXECUTABLE="c-wire"

if [ ! -f "$FICHIER_DAT" ]; then
    echo "Erreur : Le fichier '$FICHIER_DAT' est introuvable."
    exit 2
fi

# Vérification de l'option pour histo
if [ "$COMMANDE" = "histo" ]; then
    if [ "$OPTION" != "max" ] && [ "$OPTION" != "src" ] && [ "$OPTION" != "real" ]; then
        echo "Erreur : Option invalide pour histo. Choisir : max, src, ou real."
        exit 3
    fi
fi

# --- COMPILATION ---
# On compile seulement si l'exécutable n'existe pas ou si on veut être sûr
if [ ! -f "$EXECUTABLE" ]; then
    echo "Compilation en cours..."
    make
    if [ $? -ne 0 ]; then
        echo "Erreur : La compilation a échoué."
        exit 1
    fi
fi

# --- EXÉCUTION DU C ET GESTION DES FICHIERS ---

# On nettoie l'ancien fichier temporaire s'il existe
rm -f stats.csv

echo "Traitement des données en cours..."
./"$EXECUTABLE" "$FICHIER_DAT" "$COMMANDE" "$OPTION"

if [ $? -ne 0 ]; then
    echo "Erreur lors de l'exécution du programme C."
    exit 1
fi

# RENOMMAGE SELON LA CONSIGNE
# On renomme stats.csv en vol_max.dat, vol_code.dat, etc.
NOM_FICHIER_SORTIE="vol_${OPTION}.csv"
mv stats.csv "$NOM_FICHIER_SORTIE"

echo "Fichier de données généré : $NOM_FICHIER_SORTIE"

# --- GNUPLOT (GÉNÉRATION D'UNE SEULE IMAGE) ---

if [ "$COMMANDE" = "histo" ]; then
    # Tri numérique pour les graphiques (Min et Max)
    # On trie par la 2ème colonne (valeur)
    sort -t';' -k2,2n "$NOM_FICHIER_SORTIE" > data_triee.tmp

    # Récupération des 5 premiers (Min) et 5 derniers (Max)
    head -n 5 data_triee.tmp > data_min.dat
    tail -n 5 data_triee.tmp > data_max.dat

    echo "Génération du graphique..."

    # Script Gnuplot pour MULTIPLOT (2 graphes en 1 image)
    gnuplot <<- EOF
        set terminal png size 800,1000
        set output 'graph_${OPTION}.png'
        set datafile separator ";"
        
        # Configuration pour mettre 2 graphiques l'un sur l'autre
        set multiplot layout 2,1 title "Statistiques : ${OPTION}" font ",14"

        # Graphique 1 : Les 5 Min
        set title "Top 5 Min"
        set style data histograms
        set style fill solid
        set ylabel "Quantité (m3)"
        # Rotation des étiquettes en bas pour qu'elles soient lisibles
        set xtics rotate by -45
        plot "data_min.dat" using 2:xtic(1) title "Volume" linecolor rgb "blue"

        # Graphique 2 : Les 5 Max
        set title "Top 5 Max"
        set style data histograms
        set style fill solid
        set ylabel "Quantité (m3)"
        set xtics rotate by -45
        plot "data_max.dat" using 2:xtic(1) title "Volume" linecolor rgb "red"

        unset multiplot
EOF

    # Nettoyage temporaire
    rm data_triee.tmp data_min.dat data_max.dat
    
    echo "Graphique généré : graph_${OPTION}.png"
fi

# --- VERIFICATION DU TRI INVERSE (Z -> A) ---
# Le fichier C a généré un fichier trié inversement par ID.
# On affiche les 5 premières lignes pour vérifier que ça commence par Z, Y, W...
echo "--- Vérification du tri inverse (Aperçu du début du fichier) ---"
head -n 5 "$NOM_FICHIER_SORTIE"
