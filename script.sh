#!/bin/bash

# Vérification des arguments
if [ $# -lt 3 ]; then
    echo "Usage: $0 <fichier_dat> <station_type> <consommateur_type>"
    exit 1
fi

FICHIER="$1"
CMD="$2"
MODE="$3"

if [ ! -f "$FICHIER" ]; then
    echo "Erreur : Fichier '$FICHIER' introuvable."
    exit 2
fi

# Compilation
make clean
make

if [ ! -f c-wire ]; then
    echo "Erreur lors de la compilation."
    exit 1
fi

# Exécution du programme C
echo "Traitement des données..."
./c-wire "$FICHIER" "$CMD" "$MODE"

if [ ! -s stats.csv ]; then
    echo "Erreur : Aucune donnée générée."
    exit 1
fi

# Gestion du fichier de sortie
OUT="vol_${MODE}.csv"
mv stats.csv "$OUT"

# Génération des graphiques
if [ "$CMD" = "histo" ]; then
    
    # Choix de la colonne (Capacité ou Consommation)
    if [ "$MODE" = "max" ]; then
        COL=2
    else
        COL=3
    fi

    # Tri numérique
    sort -t';' -k${COL},${COL}n "$OUT" > temp_tri.csv
    
    # --- MODIFICATION ICI : 50 MIN et 10 MAX ---
    head -n 50 temp_tri.csv > min_data.csv
    tail -n 10 temp_tri.csv > max_data.csv

    gnuplot <<- EOF
        # On garde une grande largeur pour que les 50 barres du haut soient lisibles
        set terminal png size 1200,1000
        set output 'graph_${MODE}.png'
        set datafile separator ";"
        set multiplot layout 2,1 title "Graphe : ${MODE} (50 Min / 10 Max)"
        
        set bmargin 8
        
        # Graphique du HAUT : Les 50 plus petits
        set title "Les 50 plus faibles"
        set style data histograms
        set style fill solid
        set ylabel "Quantité (m3)"
        set xtics rotate by -90 font ",8"
        plot "min_data.csv" using ${COL}:xtic(1) title "Volume" lc rgb "blue"

        # Graphique du BAS : Les 10 plus grands
        set title "Les 10 plus forts"
        set style data histograms
        set style fill solid
        set ylabel "Quantité (m3)"
        # On garde la même rotation pour la cohérence
        set xtics rotate by -90 font ",8"
        plot "max_data.csv" using ${COL}:xtic(1) title "Volume" lc rgb "red"
        
        unset multiplot
EOF
    
    rm temp_tri.csv min_data.csv max_data.csv
    echo "Graphique généré : graph_${MODE}.png"
fi
