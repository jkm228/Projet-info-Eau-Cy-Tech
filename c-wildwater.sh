#!/bin/bash

# ===================================================================
# PROJET C-WILDWATER - Script de Traitement
# ===================================================================

# 0. DEBUT CHRONO (Millisecondes)
# "date +%s%3N" donne le temps en ms
START_TIME=$(date +%s%3N)

# Fonction pour afficher le temps et quitter
fin_script() {
    END_TIME=$(date +%s%3N)
    DURATION=$((END_TIME - START_TIME))
    # Si le calcul donne un nombre négatif (rare mais possible selon OS), on met 0
    if [ "$DURATION" -lt 0 ]; then DURATION=0; fi
    echo "Durée totale : ${DURATION} ms"
    exit $1
}

# 1. GESTION DE L'AIDE (-h)
if [ "$1" = "-h" ]; then
    echo "Usage: $0 <fichier_dat> <commande> [arg]"
    echo "Commandes :"
    echo "  histo <max|src|real>  : Histogrammes"
    echo "  leaks <Identifiant>   : Fuites d'une usine"
    fin_script 0
fi

# 2. VERIFICATION ARGUMENTS DE BASE
if [ $# -lt 3 ]; then
    echo "Erreur : Nombre d'arguments insuffisant."
    fin_script 1
fi

FICHIER_DAT="$1"
COMMANDE="$2"
ARG_3="$3"

if [ ! -f "$FICHIER_DAT" ]; then
    echo "Erreur : Fichier '$FICHIER_DAT' introuvable."
    fin_script 2
fi

# 3. VERIFICATION ET COMPILATION
if [ ! -f "Makefile" ]; then
    echo "Erreur : Makefile introuvable."
    fin_script 3
fi

# On vérifie si l'exécutable existe, sinon on compile
if [ ! -x "c-wire" ]; then
    echo "Compilation en cours..."
    make > /dev/null
    if [ $? -ne 0 ]; then
        echo "Erreur : Echec de la compilation."
        fin_script 3
    fi
fi

FICHIER_TMP="input_data.tmp"
FICHIER_STATS="stats.csv"

# 4. AIGUILLAGE SELON LA COMMANDE
if [ "$COMMANDE" = "histo" ]; then
    # --- MODE HISTO ---
    
    # Vérification pas d'arguments en trop
    if [ $# -ne 3 ]; then
        echo "Erreur : Trop d'arguments pour 'histo'."
        fin_script 1
    fi

    echo "Traitement 'histo' en mode '$ARG_3'..."

    case "$ARG_3" in
        "max")
            awk -F';' '$2 ~ "Plant" && $4 != "-" {printf "%s;%.6f;0\n", $2, $4/1000000}' "$FICHIER_DAT" > "$FICHIER_TMP"
            HEADER="identifier;max volume (M.m3.year-1)" # M.m3 demandé dans la consigne précédente
            FICHIER_SORTIE="vol_max.dat"
            TITRE_GRAPH="Capacité Maximale"
            COL_VAL=2
            ;;
        "src")
            awk -F';' '$2 ~ "Source|Well|Resurgence|Spring|Fountain" && $3 ~ "Plant" {printf "%s;0;%.6f\n", $3, $4/1000000}' "$FICHIER_DAT" > "$FICHIER_TMP"
            HEADER="identifier;source volume (M.m3.year-1)"
            FICHIER_SORTIE="vol_captation.dat"
            TITRE_GRAPH="Volume Capté Total"
            COL_VAL=3
            ;;
        "real")
            awk -F';' '$2 ~ "Source|Well|Resurgence|Spring|Fountain" && $3 ~ "Plant" {
                vol = $4; fuite = ($5 == "-" ? 0 : $5); reel = vol * (1 - fuite/100);
                printf "%s;0;%.6f\n", $3, reel/1000000
            }' "$FICHIER_DAT" > "$FICHIER_TMP"
            HEADER="identifier;real volume (M.m3.year-1)"
            FICHIER_SORTIE="vol_traitement.dat"
            TITRE_GRAPH="Volume Traité Net"
            COL_VAL=3
            ;;
        *)
            echo "Erreur : Mode '$ARG_3' inconnu pour histo."
            fin_script 1
            ;;
    esac

    # Exécution C (Mode global)
    ./c-wire "$FICHIER_TMP"
    if [ $? -ne 0 ]; then echo "Erreur C"; fin_script 4; fi

    # Tri et Sortie
    echo "$HEADER" > "$FICHIER_SORTIE"
    awk -F';' -v col=$COL_VAL 'NR>1 {print $1";"$(col)}' "$FICHIER_STATS" | sort -t';' -k1,1r >> "$FICHIER_SORTIE"

    # GnuPlot
    echo "Génération graphique..."
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
        set ylabel "Volume (M.m3)"
        set title "Top 10 Usines"
        set xtics rotate by -45
        set boxwidth 0.7
        set grid y
        plot "data_max.dat" using 2:xtic(1) notitle linecolor rgb "#006400"
        set title "Les 50 plus petites"
        unset xtics 
        plot "data_min.dat" using 2 notitle linecolor rgb "#FF4500"
        unset multiplot
EOF
    rm graph_data.sorted data_min.dat data_max.dat

elif [ "$COMMANDE" = "leaks" ]; then
    # --- MODE LEAKS ---

    # Vérification pas d'arguments en trop
    if [ $# -ne 3 ]; then
        echo "Erreur : Trop d'arguments pour 'leaks' ou ID manquant."
        fin_script 1
    fi

    TARGET_ID="$ARG_3"
    echo "Analyse des fuites pour '$TARGET_ID'..."

    # On prépare les données pour le C
    # On cherche les lignes où l'usine est la SOURCE du tuyau (col 1)
    # Le calcul de la fuite brute est : Volume (col 4) * Taux (col 5) / 100
    # On envoie au C : ID_USINE ; 0 ; VALEUR_FUITE
    # Note : Le C va sommer ces fuites.
    
    awk -F';' -v id="$TARGET_ID" '
    $1 == id && $5 != "-" {
        vol = $4;
        taux = $5;
        perte = vol * (taux/100);
        # M.m3 demandé en sortie
        printf "%s;0;%.6f\n", $1, perte/1000000
    }' "$FICHIER_DAT" > "$FICHIER_TMP"

    # Exécution C avec l'ID en argument !
    ./c-wire "$FICHIER_TMP" "$TARGET_ID"
    CODE_RETOUR=$?

    if [ $CODE_RETOUR -ne 0 ]; then
        echo "Erreur lors de l'exécution du programme C."
        fin_script 4
    fi

    FICHIER_SORTIE="leaks.csv" # Nom non précisé, leaks.csv est logique
    echo "identifier;Leak volume (M.m3.year-1)" > "$FICHIER_SORTIE"
    
    # Le C a écrit dans stats.csv soit "ID;VAL" soit "ID;-1"
    cat "$FICHIER_STATS" >> "$FICHIER_SORTIE"

    echo "Fichier généré : $FICHIER_SORTIE"
    if grep -q "\-1$" "$FICHIER_SORTIE"; then
        echo "Attention : Usine introuvable ou aucune fuite détectée (Volume -1)."
    fi

else
    echo "Erreur : Commande '$COMMANDE' inconnue."
    fin_script 1
fi

# Nettoyage
rm -f "$FICHIER_TMP" "$FICHIER_STATS"

# 5. FIN AVEC TEMPS
fin_script 0
