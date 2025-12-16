#!/bin/bash

# ===================================================================
# PROJET C-WILDWATER - Script de Traitement (Conforme Consignes)
# ===================================================================

# 1. GESTION DE L'AIDE (-h)
if [ "$1" = "-h" ]; then
    echo "Usage: $0 <fichier_dat> <mode_usine>"
    echo ""
    echo "Modes pour les usines :"
    echo "  cap     : Capacité maximale (Millions m3)"
    echo "  flow    : Volume capté depuis les sources (Millions m3)"
    echo "  treat   : Volume traité (après fuites) (Millions m3)"
    exit 0
fi

# 2. VERIFICATION DES ARGUMENTS
if [ $# -lt 2 ]; then
    echo "Erreur : Arguments manquants."
    echo "Usage: $0 <fichier_dat> <mode>"
    exit 1
fi

FICHIER_DAT="$1"
MODE="$2"

if [ ! -f "$FICHIER_DAT" ]; then
    echo "Erreur : Fichier '$FICHIER_DAT' introuvable."
    exit 2
fi

# 3. COMPILATION (Sécurité)
if [ -f "Makefile" ]; then
    make clean > /dev/null
    make > /dev/null
    if [ $? -ne 0 ]; then echo "Erreur Compilation"; exit 3; fi
else
    echo "Erreur : Makefile introuvable."; exit 3
fi

# 4. FILTRAGE ET CALCULS (C'est ici qu'on gère les consignes !)
# On convertit tout en Millions de m3 (/1000000) dès le début.

FICHIER_TMP="input_data.tmp"
echo "Préparation des données pour le mode '$MODE'..."

case "$MODE" in
    "cap")
        # On cherche les usines (Plant) définies (col 2) avec leur capacité (col 4)
        # Format C : ID;CAPACITE;0
        awk -F';' '$2 ~ "Plant" && $4 != "-" {printf "%s;%.6f;0\n", $2, $4/1000000}' "$FICHIER_DAT" > "$FICHIER_TMP"
        TITRE_GRAPH="Capacité Maximale (Mm3)"
        FICHIER_SORTIE="vol_max.dat"
        ;;
        
    "flow")
        # On cherche les flux Sources -> Usine
        # Le flux est défini par une source (Source|Well|... en col 2) vers une usine (Plant en col 3)
        # Format C : ID_USINE;0;VOLUME
        awk -F';' '$2 ~ "Source|Well|Resurgence|Spring|Fountain" && $3 ~ "Plant" {printf "%s;0;%.6f\n", $3, $4/1000000}' "$FICHIER_DAT" > "$FICHIER_TMP"
        TITRE_GRAPH="Volume Capté Total (Mm3)"
        FICHIER_SORTIE="vol_captation.dat"
        ;;

    "treat")
        # On cherche les flux Sources -> Usine MAIS on applique les fuites
        # Col 4 = Volume, Col 5 = Fuites (%)
        # Formule : Vol * (1 - Fuite/100)
        awk -F';' '
        $2 ~ "Source|Well|Resurgence|Spring|Fountain" && $3 ~ "Plant" {
            vol = $4;
            fuite = ($5 == "-" ? 0 : $5);
            reel = vol * (1 - fuite/100);
            printf "%s;0;%.6f\n", $3, reel/1000000
        }' "$FICHIER_DAT" > "$FICHIER_TMP"
        TITRE_GRAPH="Volume Traité Net (Mm3)"
        FICHIER_SORTIE="vol_traitement.dat"
        ;;
    *)
        echo "Erreur : Mode '$MODE' inconnu (choisir: cap, flow, treat)."
        exit 1
        ;;
esac

# 5. EXECUTION DU PROGRAMME C
# Le C fait la somme des valeurs par station
echo "Exécution du traitement C..."
./c-wire "$FICHIER_TMP"

if [ ! -s "stats.csv" ]; then
    echo "Erreur : Le programme C n'a rien produit."
    exit 4
fi

# 6. TRI FINAL ET FORMATAGE (Consigne : Alphabétique Inverse)
echo "Génération du fichier $FICHIER_SORTIE trié (Alpha Inverse)..."

# On ajoute l'en-tête spécifique
echo "Station;Valeur(Mm3)" > "$FICHIER_SORTIE"

# Tri inverse sur la 1ère colonne (ID)
# Le fichier stats.csv du C est : ID;CAP;CONSO. 
# Selon le mode, on veut soit la CAP (col 2) soit la CONSO (col 3)
if [ "$MODE" = "cap" ]; then
    COL_VAL=2
else
    COL_VAL=3
fi

# On extrait ID et la bonne colonne, puis on trie Z->A (sort -r)
awk -F';' -v col=$COL_VAL 'NR>1 {print $1";"$(col)}' stats.csv | sort -t';' -k1,1r >> "$FICHIER_SORTIE"

# Nettoyage temporaire
rm "$FICHIER_TMP" "stats.csv"

# 7. GENERATION GNUPLOT (Consigne : 10 plus gros, 50 plus petits)
echo "Génération du graphique..."

# Préparation des données pour Gnuplot (Tri numérique cette fois)
# On exclut le header
tail -n +2 "$FICHIER_SORTIE" | sort -t';' -k2,2n > graph_data.sorted

# Les 50 plus petits (tête du fichier trié croissant)
head -n 50 graph_data.sorted > data_min.dat
# Les 10 plus grands (queue du fichier trié croissant)
tail -n 10 graph_data.sorted > data_max.dat

gnuplot -persist <<-EOF
    set terminal png size 1200,800 enhanced font "Arial,10"
    set output '${FICHIER_SORTIE%.*}.png'
    set datafile separator ";"
    set multiplot layout 1,2 title "Statistiques : ${TITRE_GRAPH}"
    
    # Graphique 1 : Les 10 plus grands
    set title "Top 10 Usines (Plus grand volume)"
    set style data histograms
    set style fill solid 1.0 border -1
    set ylabel "Volume (Mm3)"
    set xtics rotate by -45
    set boxwidth 0.7
    set grid y
    plot "data_max.dat" using 2:xtic(1) notitle linecolor rgb "#006400"

    # Graphique 2 : Les 50 plus petits
    set title "Les 50 plus petites Usines"
    # On enlève les xtics car 50 noms c'est illisible
    unset xtics 
    set xlabel "Usines (Identifiants masqués pour lisibilité)"
    plot "data_min.dat" using 2 notitle linecolor rgb "#FF4500"

    unset multiplot
EOF

# Nettoyage fichiers gnuplot
rm graph_data.sorted data_min.dat data_max.dat

echo "Terminé ! Fichiers générés :"
echo " - Données : $FICHIER_SORTIE"
echo " - Image   : ${FICHIER_SORTIE%.*}.png"
