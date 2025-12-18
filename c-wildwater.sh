#!/bin/bash

# ===================================================================
# PROJET C-WILDWATER - Script de Traitement (FINAL - CONFORME PDF)
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

# Compilation automatique si nécessaire
if [ ! -x "c-wire" ]; then
    make > /dev/null
    if [ $? -ne 0 ]; then echo "Erreur Compilation"; fin_script 3; fi
fi

FICHIER_TMP="input_data.tmp"
FICHIER_STATS="stats.csv"

# 3. TRAITEMENT
if [ "$COMMANDE" = "histo" ]; then
    
    if [ $# -ne 3 ]; then echo "Erreur Args"; fin_script 1; fi

    # Préparation des données selon le mode (Division par 1 000 000 pour M.m3)
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

    # Traitement C (AVL)
    ./c-wire "$FICHIER_TMP"
    if [ $? -ne 0 ]; then echo "Erreur C"; fin_script 4; fi

    # Génération du fichier de sortie trié
    echo "$HEADER" > "$FICHIER_SORTIE"
    
    # --- CORRECTION VALIDEE ---
    # Tri Numérique (n) et Décroissant (r) sur la 2ème colonne (k2)
    # Les plus grandes valeurs seront en haut du fichier .dat
    awk -F';' -v c=$COL 'NR>1 {print $1";"$(c)}' "$FICHIER_STATS" | sort -t';' -k2,2nr >> "$FICHIER_SORTIE"

    # Préparation Gnuplot
    # On retrie en croissant (n) temporairement pour extraire facilement min/max pour le graph
    tail -n +2 "$FICHIER_SORTIE" | sort -t';' -k2,2n > graph.tmp
    head -n 50 graph.tmp > min.dat  # Les 50 plus petits
    tail -n 10 graph
