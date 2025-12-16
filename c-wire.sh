#!/bin/bash

# ===================================================================
# 0. VÉRIFICATION DES ARGUMENTS ET COMPILATION
# ===================================================================

if [ $# -lt 3 ]; then
    echo "Usage: $0 <fichier_dat> <station_type> <consommateur_type>"
    echo "Exemple: $0 c-wildwater_v0.dat histo max"
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

# ===================================================================
# 1. EXÉCUTION DU PROGRAMME C
# ===================================================================
echo "Traitement des données en cours pour le mode : $MODE"
./c-wire "$FICHIER" "$CMD" "$MODE"

if [ ! -s stats.csv ]; then
    echo "Erreur : Le fichier stats.csv est vide ou n'a pas été généré."
    exit 1
fi

# ===================================================================
# 2. NETTOYAGE ET TRI (CORRIGÉ : PLUS DE GREP "PLANT")
# ===================================================================
OUT="vol_${MODE}.csv"

# A. Isolation de l'en-tête
head -n 1 stats.csv > header.csv

# B. On garde toutes les lignes générées par le C (sauf l'en-tête)
# Le programme C a déjà fait le travail de filtrage !
tail -n +2 stats.csv > data_filtered.csv

# C. Tri final alphabétique inverse (Z->A) pour le CSV de sortie
sort -t';' -k1,1r data_filtered.csv > data_sorted.csv

# D. Création du fichier final
cat header.csv data_sorted.csv > "$OUT"
echo "Fichier CSV généré : $OUT"

rm stats.csv header.csv data_sorted.csv

# ===================================================================
# 3. GRAPHIQUE
# ===================================================================

if [ "$CMD" = "histo" ]; then

    # Sélection de la colonne selon le mode
    # Format CSV du C : ID ; CAPACITE ; CONSO
    case "$MODE" in
        "max")
            COL=2 # Capacité
            TITRE="Capacité Max (HVB/Stations)"
            ;;
        "src")
            COL=3 # Flux entrant (stocké dans Conso par avl.c pour src)
            TITRE="Volume Capté (Source -> Usine)"
            ;;
        "real"|"lv") 
            COL=3 # Consommation
            TITRE="Volume Traité (Clients)"
            ;;
        *)
            COL=2
            TITRE="Graphique $MODE"
            ;;
    esac

    # 1. Tri NUMÉRIQUE croissant pour l'affichage Gnuplot
    sort -t';' -k${COL},${COL}n data_filtered.csv > temp_graph_data.csv

    # 2. Extraction Min/Max
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

    # 3. Gnuplot (Ajusté pour petits écrans et échelle)
    gnuplot <<- EOF
        set terminal png size 1200,1000 enhanced font "arial,10"
        set output 'graph_${MODE}.png'
        set datafile separator ";"
        
        set multiplot layout 2,1 title "${TITRE}"
        
        set grid y
        set style data histograms
        set style fill solid 1.0 border -1
        set boxwidth 0.7
        
        # Axe Y formaté pour éviter la notation scientifique
        set format y "%.2f"
        
        # --- GRAPHIQUE 1 : Les plus faibles ---
        set title "Les plus faibles (Min)"
        set ylabel "M.m3"
        set xtics rotate by -45 font ",8"
        # Division par 1000000 pour M.m3
        plot "min_data.csv" using (\$${COL}/1000000):xtic(1) notitle lc rgb "#3366CC"

        # --- GRAPHIQUE 2 : Les plus forts ---
        set title "Les plus forts (Max)"
        set ylabel "M.m3"
        set xtics rotate by -45 font ",8"
        plot "max_data.csv" using (\$${COL}/1000000):xtic(1) notitle lc rgb "#DC3912"
        
        unset multiplot
EOF
    
    rm temp_graph_data.csv min_data.csv max_data.csv data_filtered.csv
    echo "Graphique généré : graph_${MODE}.png"
fi
