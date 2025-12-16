#!/bin/bash

# ===================================================================
# PROJET C-WILDWATER - Script de Traitement (FINAL - DEDUPLICATION)
# ===================================================================

# 0. DEBUT CHRONO
START_TIME=$(date +%s%3N)

fin_script() {
    END_TIME=$(date +%s%3N)
    DURATION=$((END_TIME - START_TIME))
    if [ "$DURATION" -lt 0 ]; then DURATION=0; fi
    echo "Durée totale : ${DURATION} ms"
    exit $1
}

# 1. GESTION AIDE
if [ "$1" = "-h" ]; then
    echo "Usage: $0 <fichier_dat> <commande> [arg]"
    echo "  histo <max|src|real>"
    echo "  leaks <Identifiant>"
    fin_script 0
fi

# 2. VERIFICATIONS
if [ $# -lt 3 ]; then
    echo "Erreur : Arguments insuffisants."
    fin_script 1
fi

FICHIER_DAT="$1"
COMMANDE="$2"
ARG_3="$3"

if [ ! -f "$FICHIER_DAT" ]; then echo "Erreur Fichier"; fin_script 2; fi
if [ ! -f "Makefile" ]; then echo "Erreur Makefile"; fin_script 3; fi

if [ ! -x "c-wire" ]; then
    echo "Compilation..."
    make > /dev/null
    if [ $? -ne 0 ]; then echo "Erreur Compilation"; fin_script 3; fi
fi

FICHIER_TMP="input_data.tmp"
FICHIER_STATS="stats.csv"

# 3. TRAITEMENT
if [ "$COMMANDE" = "histo" ]; then
    
    if [ $# -ne 3 ]; then echo "Erreur Args"; fin_script 1; fi
    echo "Traitement Histo '$ARG_3'..."

    # Pour l'histo, pas besoin de déduplication complexe car on somme des capacités/volumes
    # qui sont définis sur des lignes spécifiques (sauf pour real où on filtre src->plant)

    case "$ARG_3" in
        "max")
            awk -F';' '$2 ~ "Plant" && $4 != "-" {printf "%s;%.6f;0\n", $2, $4/1000000}' "$FICHIER_DAT" > "$FICHIER_TMP"
            HEADER="identifier;max volume (M.m3.year-1)"
            FICHIER_SORTIE="vol_max.dat"
            TITRE="Capacité Maximale"
            COL=2
            ;;
        "src")
            awk -F';' '$2 ~ "Source|Well|Resurgence|Spring|Fountain" && $3 ~ "Plant" {printf "%s;0;%.6f\n", $3, $4/1000000}' "$FICHIER_DAT" > "$FICHIER_TMP"
            HEADER="identifier;source volume (M.m3.year-1)"
            FICHIER_SORTIE="vol_captation.dat"
            TITRE="Volume Capté"
            COL=3
            ;;
        "real")
            awk -F';' '$2 ~ "Source|Well|Resurgence|Spring|Fountain" && $3 ~ "Plant" {
                vol = $4; fuite = ($5 == "-" ? 0 : $5); reel = vol * (1 - fuite/100);
                printf "%s;0;%.6f\n", $3, reel/1000000
            }' "$FICHIER_DAT" > "$FICHIER_TMP"
            HEADER="identifier;real volume (M.m3.year-1)"
            FICHIER_SORTIE="vol_traitement.dat"
            TITRE="Volume Traité"
            COL=3
            ;;
        *) echo "Erreur Mode"; fin_script 1 ;;
    esac

    ./c-wire "$FICHIER_TMP"
    if [ $? -ne 0 ]; then echo "Erreur C"; fin_script 4; fi

    echo "$HEADER" > "$FICHIER_SORTIE"
    awk -F';' -v c=$COL 'NR>1 {print $1";"$(c)}' "$FICHIER_STATS" | sort -t';' -k1,1r >> "$FICHIER_SORTIE"

    # GnuPlot
    tail -n +2 "$FICHIER_SORTIE" | sort -t';' -k2,2n > graph.tmp
    head -n 50 graph.tmp > min.dat
    tail -n 10 graph.tmp > max.dat

    gnuplot -persist <<-EOF
        set terminal png size 1200,800 enhanced font "Arial,10"
        set output '${FICHIER_SORTIE%.*}.png'
        set datafile separator ";"
        set multiplot layout 1,2 title "Statistiques : ${TITRE}"
        set style data histograms
        set style fill solid 1.0 border -1
        set ylabel "Volume (M.m3)"
        set title "Top 10"
        set xtics rotate by -45
        plot "max.dat" using 2:xtic(1) notitle linecolor rgb "#006400"
        set title "Bottom 50"
        unset xtics
        plot "min.dat" using 2 notitle linecolor rgb "#FF4500"
        unset multiplot
EOF
    rm graph.tmp min.dat max.dat

elif [ "$COMMANDE" = "leaks" ]; then

    if [ $# -ne 3 ]; then echo "Erreur Args"; fin_script 1; fi
    TARGET="$ARG_3"
    echo "Analyse Fuites pour '$TARGET'..."

    # --- EXTRACTION AVEC DÉDUPLICATION (CORRECTION MAJEURE) ---
    
    awk -F';' '
    BEGIN {OFS=";"}
    
    # 1. Connexions Source -> Usine (Col 2 -> Col 3)
    # Celles-ci sont uniques par définition dans le fichier, mais on sécurise.
    $2 ~ "Source|Well|Resurgence|Spring|Fountain" && $3 ~ "Plant" { 
        pair = $2 ";" $3
        if (!seen[pair]++) {
            print $2, $3, $4/1000000, ($5=="-"?0:$5) 
        }
    }
    
    # 2. Connexions Usine -> Stockage/Service (Col 1 -> Col 2)
    # Cest ICI que se trouvaient les milliers de doublons
    $1 ~ "Plant" && ($2 ~ "Storage" || $2 ~ "Service") {
        pair = $1 ";" $2
        if (!seen[pair]++) {
            # Le volume est inconnu ici (0), la fuite est en col 4 ou 5 selon le type
            leak = ($4=="-" ? ($5=="-"?0:$5) : $4)
            print $1, $2, 0, leak
        }
    }

    # 3. Connexions Service -> Client ou Stockage -> Jonction (Col 2 -> Col 3)
    # On prend tout ce qui reste de logique : Si Col 2 et Col 3 existent
    $2 != "" && $2 != "-" && $3 != "" && $3 != "-" && $2 !~ "Source|Well" {
        pair = $2 ";" $3
        if (!seen[pair]++) {
             print $2, $3, 0, ($5=="-"?0:$5)
        }
    }
    ' "$FICHIER_DAT" > "$FICHIER_TMP"

    ./c-wire "$FICHIER_TMP" "$TARGET"
    if [ $? -ne 0 ]; then echo "Erreur C"; fin_script 4; fi

    FICHIER_SORTIE="leaks.csv"
    echo "identifier;Leak volume (M.m3.year-1)" > "$FICHIER_SORTIE"
    cat "$FICHIER_STATS" >> "$FICHIER_SORTIE"
    
    if grep -q "\-1$" "$FICHIER_SORTIE"; then
        echo "Info : Usine introuvable ou -1."
    fi

else
    echo "Erreur Commande"; fin_script 1
fi

rm -f "$FICHIER_TMP" "$FICHIER_STATS"
fin_script 0
