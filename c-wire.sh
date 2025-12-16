#!/bin/bash


if [ $# -lt 3 ]; then
    echo "Usage: $0 <fichier_dat> <station_type> <consommateur_type>"
    echo "Exemple: $0 c-wildwater_v3.dat histo max"
    exit 1
fi

FICHIER="$1"
CMD="$2"
MODE="$3"

if [ ! -f "$FICHIER" ]; then
    echo "Erreur : Fichier '$FICHIER' introuvable."
    exit 2
fi



if [ ! -x c-wire ]; then
    make clean
    make
fi

if [ ! -x c-wire ]; then
    echo "Erreur : La compilation a échoué."
    exit 1
fi


echo "Traitement des données en cours..."
./c-wire "$FICHIER" "$CMD" "$MODE"

if [ ! -s stats.csv ]; then
    echo "Erreur : Le fichier stats.csv est vide ou n'a pas été généré."
    exit 1
fi


OUT="vol_${MODE}.csv"


head -n 1 stats.csv > header.csv


tail -n +2 stats.csv | grep "Plant" > data_filtered.csv


sort -t';' -k1,1r data_filtered.csv > data_sorted.csv

cat header.csv data_sorted.csv > "$OUT"

echo "Fichier CSV généré : $OUT"

rm stats.csv header.csv data_sorted.csv


if [ "$CMD" = "histo" ]; then
    
    # Configuration des colonnes
    # Max = Capacité (col 2), Autres = Consommation (col 3)
    if [ "$MODE" = "max" ]; then
        COL=2
        Y_LABEL="Capacité (M.m3)"
    else
        COL=3
        Y_LABEL="Consommation (M.m3)"
    fi


    sort -t';' -k${COL},${COL}n data_filtered.csv > temp_graph_data.csv

 
    NB_LIGNES=$(wc -l < temp_graph_data.csv)
    
    if [ "$NB_LIGNES" -ge 50 ]; then
        head -n 50 temp_graph_data.csv > min_data.csv
    else
        cat temp_graph_data.csv > min_data.csv
    fi

    if [ "$NB_LIGNES" -ge 10 ]; then
        tail -n 10 temp_graph_data.csv > max_data.csv
    else
        cat temp_graph_data.csv > max_data.csv
    fi

    
    gnuplot <<- EOF
        set terminal png size 1200,1000
        set output 'graph_${MODE}.png'
        set datafile separator ";"
        set multiplot layout 2,1 title "Histogramme de performance des usines (${MODE})"
        
        set bmargin 8
        set grid y
        set format y "%.6f" 
        
        # --- GRAPHIQUE 1 : Les Min ---
        set title "Les 50 plus faibles"
        set style data histograms
        set style fill solid
        set ylabel "${Y_LABEL}"
        set xtics rotate by -90 font ",8"
        
        # ATTENTION : Ici on divise la colonne par 1 000 000 pour avoir des M.m3
        plot "min_data.csv" using (\$${COL}/1000000):xtic(1) title "Volume" lc rgb "blue"

        # --- GRAPHIQUE 2 : Les Max ---
        set title "Les 10 plus forts"
        set style data histograms
        set style fill solid
        set ylabel "${Y_LABEL}"
        set xtics rotate by -90 font ",8"
        
        # Idem : division par 1 000 000
        plot "max_data.csv" using (\$${COL}/1000000):xtic(1) title "Volume" lc rgb "red"
        
        unset multiplot
EOF
    
    # Nettoyage final
    rm temp_graph_data.csv min_data.csv max_data.csv data_filtered.csv
    echo "Graphique généré : graph_${MODE}.png"
fi
