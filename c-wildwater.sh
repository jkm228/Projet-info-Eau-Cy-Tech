#!/bin/bash

# ===================================================================
# PROJET C-WILDWATER - Script de Traitement
# ===================================================================

# 1. GESTION DE L'AIDE (-h)
if [ "$1" = "-h" ]; then
    echo "Usage: $0 <fichier_dat> histo <mode>"
    echo ""
    echo "Commandes :"
    echo "  histo : Génère un histogramme des usines."
    echo ""
    echo "Modes disponibles pour 'histo' :"
    echo "  max   : Capacité maximale (max volume)"
    echo "  src   : Volume capté par les sources (source volume)"
    echo "  real  : Volume réellement traité (real volume)"
    exit 0
fi

# 2. VERIFICATION DES ARGUMENTS
# On attend maintenant 3 arguments : Fichier, "histo", Mode
if [ $# -lt 3 ]; then
    echo "Erreur : Arguments manquants."
    echo "Usage: $0 <fichier_dat> histo <mode>"
    exit 1
fi

FICHIER_DAT="$1"
COMMANDE="$2"
MODE="$3"

# Vérification du fichier
if [ ! -f "$FICHIER_DAT" ]; then
    echo "Erreur : Fichier '$FICHIER_DAT' introuvable."
    exit 2
fi

# Vérification de la commande "histo"
if [ "$COMMANDE" != "histo" ]; then
    echo "Erreur : Commande '$COMMANDE' inconnue. Seul 'histo' est supporté."
    exit 1
fi

# 3. COMPILATION
if [ -f "Makefile" ]; then
    make clean > /dev/null
    make > /dev/null
    if [ $? -ne 0 ]; then echo "Erreur Compilation"; exit 3; fi
else
    echo "Erreur : Makefile introuvable."; exit 3
fi

# 4. FILTRAGE ET CALCULS
# Le fichier .dat est converti en format simple pour le C
FICHIER_TMP="input_data.tmp"
echo "Traitement 'histo' en mode '$MODE'..."

case "$MODE" in
    "max")
        # Ancien 'cap' -> max
        # Cherche "Plant" et sa capacité
        awk -F';' '$2 ~ "Plant" && $4 != "-" {printf "%s;%.6f;0\n", $2, $4/1000000}' "$FICHIER_DAT" > "$FICHIER_TMP"
        
        HEADER="identifier;max volume (k.m3.year-1)"
        FICHIER_SORTIE="vol_max.dat"
        TITRE_GRAPH="Capacité Maximale"
        ;;
        
    "src")
        # Ancien 'flow' -> src
        # Cherche flux Source -> Plant
        awk -F';' '$2 ~ "Source|Well|Resurgence|Spring|Fountain" && $3 ~ "Plant" {printf "%s;0;%.6f\n", $3, $4/1000000}' "$FICHIER_DAT" > "$FICHIER_TMP"
        
        HEADER="identifier;source volume (k.m3.year-1)"
        FICHIER_SORTIE="vol_captation.dat"
        TITRE_GRAPH="Volume Capté Total"
        ;;

    "real")
        # Ancien 'treat' -> real
        # Cherche flux Source -> Plant avec déduction des fuites
        awk -F';' '
        $2 ~ "Source|Well|Resurgence|Spring|Fountain" && $3 ~ "Plant" {
            vol = $4;
            fuite = ($5 == "-" ? 0 : $5);
            reel = vol * (1 - fuite/100);
            printf "%s;0;%.6f\n", $3, reel/1000000
        }' "$FICHIER_DAT" > "$FICHIER_TMP"
        
        HEADER="identifier;real volume (k.m3.year-1)"
        FICHIER_SORTIE="vol_traitement.dat"
        TITRE_GRAPH="Volume Traité Net"
        ;;
    *)
        echo "Erreur : Mode '$MODE' inconnu (choisir: max, src, real)."
        exit 1
        ;;
esac

# 5. EXECUTION DU C
# (Le C ne change pas, il fait toujours des sommes)
./c-wire "$FICHIER_TMP"

if [ ! -s "stats.csv" ]; then
    echo "Erreur : Le programme C n'a rien produit."
    exit 4
fi

# 6. TRI ET FORMATAGE DE SORTIE
echo "Génération du fichier $FICHIER_SORTIE..."

# Ecriture de l'en-tête spécifique demandé
echo "$HEADER" > "$FICHIER_SORTIE"

# Sélection de la colonne de valeur (2 ou 3) selon le fichier stats.csv généré par le C
if [ "$MODE" = "max" ]; then
    COL_VAL=2
else
    COL_VAL=3
fi

# Tri alphabétique inverse (Z->A) sur l'identifiant
awk -F';' -v col=$COL_VAL 'NR>1 {print $1";"$(col)}' stats.csv | sort -t';' -k1,1r >> "$FICHIER_SORTIE"

rm "$FICHIER_TMP" "stats.csv"

# 7. GENERATION GNUPLOT
echo "Génération du graphique..."

tail -n +2 "$FICHIER_SORTIE" | sort -t';' -k2,2n > graph_data.sorted
head -n 50 graph_data.sorted > data_min.dat
tail -n 10 graph_data.sorted > data_max.dat

gnuplot -persist <<-EOF
    set terminal png size 1200,800 enhanced font "Arial,10"
    set output '${FICHIER_SORTIE%.*}.png'
    set datafile separator ";"
    set multiplot layout 1,2 title "Statistiques : ${TITRE_GRAPH}"
    
    set style data histograms
    set style fill solid 1.0 border -1
    set ylabel "Volume (Millions m3)"
    
    # Graphique Max 10
    set title "Top 10 Usines"
    set xtics rotate by -45
    set boxwidth 0.7
    set grid y
    plot "data_max.dat" using 2:xtic(1) notitle linecolor rgb "#006400"

    # Graphique Min 50
    set title "Les 50 plus petites"
    unset xtics 
    set xlabel "Usines (IDs masqués)"
    plot "data_min.dat" using 2 notitle linecolor rgb "#FF4500"

    unset multiplot
EOF

rm graph_data.sorted data_min.dat data_max.dat

echo "Terminé. Résultats : $FICHIER_SORTIE et ${FICHIER_SORTIE%.*}.png"
