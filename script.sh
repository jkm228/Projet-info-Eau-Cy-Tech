#!/bin/bash

# --- VÉRIFICATIONS ---
if [ $# -lt 2 ]; then
    echo "Usage: $0 <fichier_dat> <station_type> [consumer_type]"
    exit 1
fi

FICHIER_DAT="$1"
COMMANDE="$2"
OPTION="$3"
EXECUTABLE="c-wire"

if [ ! -f "$FICHIER_DAT" ]; then
    echo "Erreur : Fichier '$FICHIER_DAT' introuvable."
    exit 2
fi

# --- COMPILATION ---
# On nettoie pour être sûr que tout est à jour
rm -f *.o "$EXECUTABLE"
make
if [ $? -ne 0 ]; then
    echo "Erreur : La compilation a échoué."
    exit 1
fi

# --- EXÉCUTION ---
NOM_FICHIER_SORTIE="vol_${OPTION}.csv"
rm -f "$NOM_FICHIER_SORTIE"

echo "Traitement des données en cours (Patience...)"
./"$EXECUTABLE" "$FICHIER_DAT" "$COMMANDE" "$OPTION"

if [ ! -s "stats.csv" ]; then
    echo "ERREUR CRITIQUE : Le programme C n'a généré aucune donnée !"
    exit 1
fi

mv stats.csv "$NOM_FICHIER_SORTIE"
echo "Fichier CSV généré : $NOM_FICHIER_SORTIE"

# --- GNUPLOT (INTELLIGENT) ---
if [ "$COMMANDE" = "histo" ]; then
    
    # DÉTERMINATION DE LA COLONNE À TRAITER
    # Si max -> Col 2 (Capacité)
    # Si src/real -> Col 3 (Volume traité)
    if [ "$OPTION" = "max" ]; then
        COL=2
    else
        COL=3
    fi

    # Tri numérique sur la bonne colonne ($COL)
    sort -t';' -k${COL},${COL}n "$NOM_FICHIER_SORTIE" > data_triee.tmp
    
    # Extraction des données
    head -n 5 data_triee.tmp > data_min.dat
    tail -n 5 data_triee.tmp > data_max.dat

    gnuplot <<- EOF
        set terminal png size 800,1000
        set output 'graph_${OPTION}.png'
        set datafile separator ";"
        set multiplot layout 2,1 title "Statistiques : ${OPTION}" font ",14"
        
        set title "Top 5 Min"
        set style data histograms
        set style fill solid
        set ylabel "Quantité (m3)"
        set xtics rotate by -45
        # On utilise la variable col pour cibler la bonne donnée
        plot "data_min.dat" using ${COL}:xtic(1) title "Volume" linecolor rgb "blue"

        set title "Top 5 Max"
        set style data histograms
        set style fill solid
        set ylabel "Quantité (m3)"
        set xtics rotate by -45
        plot "data_max.dat" using ${COL}:xtic(1) title "Volume" linecolor rgb "red"
        unset multiplot
EOF
    rm data_triee.tmp data_min.dat data_max.dat
    echo "Graphique généré : graph_${OPTION}.png"
fi

# --- VÉRIFICATION ---
echo "--- Début du fichier trié (Vérification Tri Inverse Z->A) ---"
head -n 3 "$NOM_FICHIER_SORTIE"
