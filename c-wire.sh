#!/bin/bash

# 1. Vérification des arguments
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

# 2. Nettoyage et Compilation
# On ne fait make clean que si nécessaire, sinon ça ralentit
if [ ! -x c-wire ]; then
    make clean
    make
fi

# Vérification ultime de la compilation
if [ ! -x c-wire ]; then
    echo "Erreur : L'exécutable 'c-wire' n'a pas été créé."
    exit 1
fi

# 3. Exécution du programme C
echo "Traitement des données en cours..."
./c-wire "$FICHIER" "$CMD" "$MODE"

if [ ! -s stats.csv ]; then
    echo "Erreur : Le fichier stats.csv est vide ou n'a pas été généré."
    exit 1
fi

# 4. Traitement du fichier de sortie (CONSIGNE SUJET)
OUT="vol_${MODE}.csv"

# Tri par Identifiant (colonne 1) en ordre Inverse (r) alphabétique
# C'est l'étape qui manquait selon ton image.
sort -t';' -k1,1r stats.csv > "$OUT"
rm stats.csv

echo "Fichier généré et trié (ID inverse) : $OUT"

# 5. Génération des graphiques (si demandé)
if [ "$CMD" = "histo" ]; then
    
    # Choix de la colonne selon le mode
    # lv all -> on regarde la consommation (col 3)
    # max -> on regarde la capacité (col 2)
    if [ "$MODE" = "max" ]; then
        COL=2
        Y_LABEL="Capacité (M.m3)"
    else
        COL=3
        Y_LABEL="Consommation (M.m3)"
    fi

    # Tri NUMÉRIQUE pour le graphique (du plus petit au plus grand volume)
    # On utilise le fichier $OUT comme source, mais on trie sur la colonne 2 ou 3
    sort -t';' -k${COL},${COL}n "$OUT" > temp_graph_data.csv

    # Extraction des 10 plus gros (fin du fichier) et 50 plus petits (début)
    head -n 50 temp_graph_data.csv > min_data.csv
    tail -n 10 temp_graph_data.csv > max_data.csv

    # Gnuplot
    gnuplot <<- EOF
        set terminal png size 1200,1000
        set output 'graph_${MODE}.png'
        set datafile separator ";"
        set multiplot layout 2,1 title "Graphique : ${MODE}"
        
        set bmargin 8
        set grid y
        
        set title "Les 50 plus faibles"
        set style data histograms
        set style fill solid
        set ylabel "${Y_LABEL}"
        set xtics rotate by -90 font ",8"
        # On plot la colonne choisie (2 ou 3) en fonction du nom (col 1)
        plot "min_data.csv" using ${COL}:xtic(1) title "Volume" lc rgb "blue"

        set title "Les 10 plus forts"
        set style data histograms
        set style fill solid
        set ylabel "${Y_LABEL}"
        set xtics rotate by -90 font ",8"
        plot "max_data.csv" using ${COL}:xtic(1) title "Volume" lc rgb "red"
        
        unset multiplot
EOF
    
    # Nettoyage des fichiers temporaires du graphique
    rm temp_graph_data.csv min_data.csv max_data.csv
    echo "Graphique généré : graph_${MODE}.png"
fi
