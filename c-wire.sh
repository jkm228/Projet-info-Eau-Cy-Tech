#!/bin/bash

# VÉRIFICATION ARGUMENTS
if [ $# -lt 3 ]; then
    echo "Usage: $0 <fichier_dat> <station_type> <consommateur_type>"
    exit 1
fi

FICHIER="$1"
CMD="$2"
MODE="$3"

# VÉRIFICATION FICHIER
if [ ! -f "$FICHIER" ]; then
    echo "Erreur : Fichier '$FICHIER' introuvable."
    exit 2
fi

# COMPILATION
make clean
make

if [ ! -x c-wire ]; then
    echo "Erreur : Compilation échouée."
    exit 1
fi

# EXÉCUTION C
echo "Traitement mode : $MODE"
./c-wire "$FICHIER" "$CMD" "$MODE"

if [ ! -s stats.csv ]; then
    echo "Erreur : stats.csv vide."
    exit 1
fi

# TRI ET FORMATAGE CSV
# On enlève l'en-tête, on trie par ID décroissant (alphabétique inverse)
head -n 1 stats.csv > header.csv
tail -n +2 stats.csv | sort -t';' -k1,1r > data_sorted.csv
cat header.csv data_sorted.csv > "vol_${MODE}.csv"

echo "Fichier CSV généré : vol_${MODE}.csv"

# GRAPHIQUE
if [ "$CMD" = "histo" ]; then
    # Choix colonne : 2 (Capacité) pour max, 3 (Conso) pour real/src
    if [ "$MODE" = "max" ]; then
        COL=2
        TITRE="Capacite Max"
    else
        COL=3
        TITRE="Volume $MODE"
    fi

    # Préparation données pour Gnuplot (tri numérique pour l'affichage)
    tail -n +2 "vol_${MODE}.csv" | sort -t';' -k${COL},${COL}n > graph_data.csv
    
    # Min (10 premiers après tri croissant) et Max (10 derniers)
    head -n 10 graph_data.csv > min_data.csv
    tail -n 10 graph_data.csv > max_data.csv

    gnuplot <<- EOF
        set terminal png size 1000,600
        set output 'graph_${MODE}.png'
        set datafile separator ";"
        set multiplot layout 1,2 title "${TITRE}"
        
        set style data histograms
        set style fill solid
        set ylabel "Quantité"
        set xtics rotate by -45 font ",8"

        set title "Les 10 plus faibles"
        plot "min_data.csv" using ${COL}:xtic(1) notitle lc rgb "blue"

        set title "Les 10 plus forts"
        plot "max_data.csv" using ${COL}:xtic(1) notitle lc rgb "red"
        
        unset multiplot
EOF
    
    echo "Graphique généré : graph_${MODE}.png"
    rm header.csv data_sorted.csv graph_data.csv min_data.csv max_data.csv stats.csv
fi
