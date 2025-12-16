#!/bin/bash

# ===================================================================
# 0. VÉRIFICATION DES ARGUMENTS ET COMPILATION
# ===================================================================

# Vérification du nombre d'arguments
if [ $# -lt 3 ]; then
    echo "Usage: $0 <fichier_dat> <station_type> <consommateur_type>"
    echo "Exemple: $0 c-wildwater_v3.dat histo max"
    echo "Types possibles : max, src, real"
    exit 1
fi

FICHIER="$1"
CMD="$2"
MODE="$3"

# Vérification de l'existence du fichier de données
if [ ! -f "$FICHIER" ]; then
    echo "Erreur : Fichier '$FICHIER' introuvable."
    exit 2
fi

# Compilation propre (uniquement si nécessaire)
if [ ! -x c-wire ]; then
    make clean
    make
fi

# Vérification que l'exécutable est bien là
if [ ! -x c-wire ]; then
    echo "Erreur : La compilation a échoué."
    exit 1
fi

# ===================================================================
# 1. EXÉCUTION DU PROGRAMME C
# ===================================================================
echo "Traitement des données en cours pour le mode : $MODE"
./c-wire "$FICHIER" "$CMD" "$MODE"

# Vérification que le CSV brut a été produit
if [ ! -s stats.csv ]; then
    echo "Erreur : Le fichier stats.csv est vide ou n'a pas été généré."
    exit 1
fi

# ===================================================================
# 2. NETTOYAGE ET FILTRAGE (COMMUN)
# ===================================================================
OUT="vol_${MODE}.csv"

# A. Isolation de l'en-tête
head -n 1 stats.csv > header.csv

# B. Filtrage : On ne garde que les USINES ("Plant")
# On ignore la 1ère ligne et on cherche le mot "Plant" pour exclure les Jonctions
tail -n +2 stats.csv | grep "Plant" > data_filtered.csv

# C. Tri final pour le CSV de sortie : Identifiant décroissant (Z->A)
# (Conforme à la consigne de tri alphabétique inverse)
sort -t';' -k1,1r data_filtered.csv > data_sorted.csv

# D. Création du fichier final
cat header.csv data_sorted.csv > "$OUT"
echo "Fichier CSV généré et trié : $OUT"

# Nettoyage des fichiers temporaires du tri CSV
rm stats.csv header.csv data_sorted.csv


# ===================================================================
# 3. CONFIGURATION DES 3 PARTIES DISTINCTES (Max, Src, Real)
# ===================================================================

if [ "$CMD" = "histo" ]; then

    # C'est ici que l'on définit les réglages spécifiques pour chaque partie
    case "$MODE" in
        
        # --- PARTIE 1 : MAX (Capacité) ---
        "max")
            COL=2
            TITRE_GRAPH="Capacité Maximale de Traitement"
            Y_LABEL="Capacité (M.m3)"
            ;;

        # --- PARTIE 2 : SRC (Volume Capté) ---
        "src")
            COL=3
            TITRE_GRAPH="Volume Total Capté (Source -> Usine)"
            Y_LABEL="Volume Capté (M.m3)"
            ;;

        # --- PARTIE 3 : REAL (Volume Traité) ---
        "real"|"lv") 
            # Note : "lv" est souvent le code pour "Load Volume" (Traité)
            COL=3
            TITRE_GRAPH="Volume Total Traité (Consommateurs)"
            Y_LABEL="Volume Traité (M.m3)"
            ;;

        *)
            echo "Attention : Mode '$MODE' inconnu pour le graphique."
            COL=3
            TITRE_GRAPH="Graphique $MODE"
            Y_LABEL="Quantité (M.m3)"
            ;;
    esac


    # ===================================================================
    # 4. GÉNÉRATION DU GRAPHIQUE (MOTEUR GNUPLOT)
    # ===================================================================
    
    # 1. Tri NUMÉRIQUE croissant pour l'affichage (Barres bleues -> Barres rouges)
    sort -t';' -k${COL},${COL}n data_filtered.csv > temp_graph_data.csv

    # 2. Extraction des Min (50 premiers) et Max (10 derniers)
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

    # 3. Lancement de Gnuplot
    gnuplot <<- EOF
        set terminal png size 1200,1000
        set output 'graph_${MODE}.png'
        set datafile separator ";"
        
        # Utilisation du titre défini dans les 3 parties
        set multiplot layout 2,1 title "${TITRE_GRAPH}"
        
        set bmargin 8
        set grid y
        # Affichage avec 6 décimales pour voir les petites valeurs
        set format y "%.6f" 
        
        # --- GRAPHIQUE HAUT : Les 50 plus faibles ---
        set title "Les 50 plus faibles"
        set style data histograms
        set style fill solid
        set ylabel "${Y_LABEL}"
        set xtics rotate by -90 font ",8"
        
        # DIVISION PAR 1 000 000 pour respecter la consigne M.m3
        plot "min_data.csv" using (\$${COL}/1000000):xtic(1) title "Volume" lc rgb "blue"

        # --- GRAPHIQUE BAS : Les 10 plus forts ---
        set title "Les 10 plus forts"
        set style data histograms
        set style fill solid
        set ylabel "${Y_LABEL}"
        set xtics rotate by -90 font ",8"
        
        # DIVISION PAR 1 000 000 ici aussi
        plot "max_data.csv" using (\$${COL}/1000000):xtic(1) title "Volume" lc rgb "red"
        
        unset multiplot
EOF
    
    # Nettoyage final
    rm temp_graph_data.csv min_data.csv max_data.csv data_filtered.csv
    echo "Graphique généré : graph_${MODE}.png"
fi
