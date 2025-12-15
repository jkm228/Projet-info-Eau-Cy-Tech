#!/bin/bash

# Arguments
FICHIER="$1"
CMD="$2"
MODE="$3"

if [ $# -lt 3 ]; then
    echo "Erreur d'arguments."
    exit 1
fi

if [ ! -f "$FICHIER" ]; then
    echo "Fichier introuvable."
    exit 2
fi

# Compilation
make clean
make

if [ ! -f c-wire ]; then
    echo "Echec compilation."
    exit 1
fi

# Execution
echo "Traitement..."
./c-wire "$FICHIER" "$CMD" "$MODE"

if [ ! -s stats.csv ]; then
    echo "Aucune donnee trouvee."
    exit 1
fi

# Renommage
OUT="vol_${MODE}.csv"
mv stats.csv "$OUT"

# Gnuplot si histo
if [ "$CMD" = "histo" ]; then
    if [ "$MODE" = "max" ]; then
        COL=2
    else
        COL=3
    fi

    # Tri par valeur numérique
    sort -t';' -k${COL},${COL}n "$OUT" > temp_tri.csv
    head -n 5 temp_tri.csv > min_data.csv
    tail -n 5 temp_tri.csv > max_data.csv

    gnuplot <<- EOF
        set terminal png size 800,1000
        set output 'graph_${MODE}.png'
        set datafile separator ";"
        set multiplot layout 2,1 title "Graphe : ${MODE}"
        
        set title "Min"
        set style data histograms
        set style fill solid
        set xtics rotate by -45
        plot "min_data.csv" using ${COL}:xtic(1) title "Vol" lc rgb "blue"

        set title "Max"
        plot "max_data.csv" using ${COL}:xtic(1) title "Vol" lc rgb "red"
        unset multiplot
EOF
    rm temp_tri.csv min_data.csv max_data.csv
fi
