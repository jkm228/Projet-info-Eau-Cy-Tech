#!/bin/bash

# ===================================================================
# PROJET C-WILDWATER - Script de Traitement
# ===================================================================

# 0. DEBUT CHRONO
START_TIME=$(date +%s%3N)

# Fonction de fin pour afficher le temps et quitter proprement
fin_script() {
    END_TIME=$(date +%s%3N)
    DURATION=$((END_TIME - START_TIME))
    # Sécurité si le calcul est négatif (rare)
    if [ "$DURATION" -lt 0 ]; then DURATION=0; fi
    echo "Durée totale : ${DURATION} ms"
    exit $1
}

# 1. GESTION DE L'AIDE (-h)
if [ "$1" = "-h" ]; then
    echo "Usage: $0 <fichier_dat> <commande> [arg]"
    echo ""
    echo "Commandes :"
    echo "  histo <max|src|real>  : Génère les statistiques et graphiques."
    echo "  leaks <Identifiant>   : Calcule les fuites pour une station."
    fin_script 0
fi

# 2. VERIFICATIONS DE BASE
if [ $# -lt 3 ]; then
    echo "Erreur : Arguments insuffisants."
    echo "Usage: $0 <fichier_dat> <commande> [arg]"
    fin_script 1
fi

FICHIER_DAT="$1"
COMMANDE="$2"
ARG_3="$3"

# Vérification présence fichier données
if [ ! -f "$FICHIER_DAT" ]; then
    echo "Erreur : Fichier '$FICHIER_DAT' introuvable."
    fin_script 2
fi

# Vérification présence Makefile
if [ ! -f "Makefile" ]; then
    echo "Erreur : Makefile introuvable."
    fin_script 3
fi

# Vérification / Compilation de l'exécutable
if [ ! -x "c-wire" ]; then
    echo "Compilation en cours..."
    make > /dev/null
    if [ $? -ne 0 ]; then 
        echo "Erreur : La compilation a échoué."
        fin_script 3
    fi
fi

# Fichiers temporaires
FICHIER_TMP="input_data.tmp"
FICHIER_STATS="stats.csv"

# 3. TRAITEMENT SELON LA COMMANDE
if [ "$COMMANDE" = "histo" ]; then
    
    # Vérification stricte du nombre d'arguments
    if [ $# -ne 3 ]; then 
        echo "Erreur : Trop d'arguments pour la commande 'histo'."
        fin_script 1
    fi

    echo "Traitement Histo en mode '$ARG_3'..."

    case "$ARG_3" in
        "max")
            # Mode MAX : On cherche les Usines (Plant) et leur capacité (Col 4)
            # Format envoyé au C : ID;CAPACITE;0
            awk -F';' '$2 ~ "Plant" && $4 != "-" {printf "%s;%.6f;0\n", $2, $4/1000000}' "$FICHIER_DAT" > "$FICHIER_TMP"
            
            # Paramètres de sortie
            HEADER="identifier;max volume (M.m3.year-1)"
            FICHIER_SORTIE="vol_max.dat"
            TITRE_GRAPH="Capacité Maximale"
            COL_VAL=2 # La valeur intéressante est en 2ème position (Capacité)
            ;;

        "src")
            # Mode SRC : Flux Source -> Usine (Col 4 = Volume)
            # Format envoyé au C : ID_USINE;0;VOLUME
            awk -F';' '$2 ~ "Source|Well|Resurgence|Spring|Fountain" && $3 ~ "Plant" {printf "%s;0;%.6f\n", $3, $4/1000000}' "$FICHIER_DAT" > "$FICHIER_TMP"
            
            HEADER="identifier;source volume (M.m3.year-1)"
            FICHIER_SORTIE="vol_captation.dat"
            TITRE_GRAPH="Volume Capté Total"
            COL_VAL=3 # La valeur intéressante est en 3ème position (Conso/Flux)
            ;;

        "real")
            # Mode REAL : Flux Source -> Usine avec déduction des fuites
            # Format envoyé au C : ID_USINE;0;VOLUME_NET
            awk -F';' '$2 ~ "Source|Well|Resurgence|Spring|Fountain" && $3 ~ "Plant" {
                vol = $4; 
                fuite = ($5 == "-" ? 0 : $5); 
                reel = vol * (1 - fuite/100);
                printf "%s;0;%.6f\n", $3, reel/1000000
            }' "$FICHIER_DAT" > "$FICHIER_TMP"
            
            HEADER="identifier;real volume (M.m3.year-1)"
            FICHIER_SORTIE="vol_traitement.dat"
            TITRE_GRAPH="Volume Traité Net"
            COL_VAL=3
            ;;

        *)
            echo "Erreur : Mode '$ARG_3' inconnu pour histo (choix : max, src, real)."
            fin_script 1 
            ;;
    esac

    # Exécution du programme C
    ./c-wire "$FICHIER_TMP"
    if [ $? -ne 0 ]; then 
        echo "Erreur lors de l'exécution du programme C."
        fin_script 4
    fi

    # Génération du fichier CSV final (Tri Alphabétique Inverse Z->A)
    echo "$HEADER" > "$FICHIER_SORTIE"
    # On trie stats.csv sur la 1ère colonne en reverse (-k1,1r)
    # Et on ne garde que ID + la Colonne de Valeur pertinente
    awk -F';' -v c=$COL_VAL 'NR>1 {print $1";"$(c)}' "$FICHIER_STATS" | sort -t';' -k1,1r >> "$FICHIER_SORTIE"

    echo "Fichier généré : $FICHIER_SORTIE"

    # Génération des Graphiques GnuPlot
    echo "Génération des graphiques..."
    
    # Préparation des données triées numériquement pour le graph
    # (On saute le header, on trie par valeur croissante)
    tail -n +2 "$FICHIER_SORTIE" | sort -t';' -k2,2n > graph.tmp
    
    # Extraction des 50 plus petits et 10 plus grands
    head -n 50 graph.tmp > min.dat
    tail -n 10 graph.tmp > max.dat

    gnuplot -persist <<-EOF
        set terminal png size 1200,800 enhanced font "Arial,10"
        set output '${FICHIER_SORTIE%.*}.png'
        set datafile separator ";"
        set multiplot layout 1,2 title "Statistiques : ${TITRE_GRAPH}"
        
        # Style des barres
        set style data histograms
        set style fill solid 1.0 border -1
        set ylabel "Volume (Millions m3)"
        
        # Graphique 1 : Top 10 (Max)
        set title "Top 10 Usines (Plus grand volume)"
        set xtics rotate by -45
        set boxwidth 0.7
        set grid y
        plot "max.dat" using 2:xtic(1) notitle linecolor rgb "#006400"

        # Graphique 2 : Bottom 50 (Min)
        set title "Les 50 plus petites Usines"
        unset xtics 
        set xlabel "Usines (IDs masqués)"
        plot "min.dat" using 2 notitle linecolor rgb "#FF4500"

        unset multiplot
EOF
    # Nettoyage fichiers temporaires Gnuplot
    rm graph.tmp min.dat max.dat

elif [ "$COMMANDE" = "leaks" ]; then
    
    # --- MODE LEAKS ---
    if [ $# -ne 3 ]; then 
        echo "Erreur : Il faut préciser l'identifiant de la station."
        fin_script 1
    fi
    
    TARGET_ID="$ARG_3"
    echo "Analyse des fuites pour la station '$TARGET_ID'..."

    # Pour leaks, on doit envoyer TOUT le graphe de connexions au C
    # Format : SOURCE ; DESTINATION ; VOLUME ; FUITE
    # Le C va reconstruire l'arbre pour calculer les pertes en aval
    
    awk -F';' '
    BEGIN {OFS=";"}
    # 1. Connexions Source -> Usine
    $2 ~ "Source|Well" && $3 ~ "Plant" { print $2, $3, $4, ($5=="-"?0:$5) }
    # 2. Connexions Usine -> Stockage
    $1 ~ "Plant" && $2 ~ "Storage" { print $1, $2, 0, ($4=="-"?0:$4) }
    # 3. Connexions Stockage -> Jonction, etc. (On prend tout ce qui a une source, une dest et pas de volume direct mais une fuite)
    $1 != "" && $2 != "" && $3 != "" && $4 == "-" { print $1, $2, 0, $5 }
    ' "$FICHIER_DAT" > "$FICHIER_TMP"

    # Exécution du C avec l'ID en paramètre
    ./c-wire "$FICHIER_TMP" "$TARGET_ID"
    if [ $? -ne 0 ]; then 
        echo "Erreur lors de l'exécution du programme C."
        fin_script 4
    fi

    # Création du fichier de sortie
    FICHIER_SORTIE="leaks.csv"
    echo "identifier;Leak volume (M.m3.year-1)" > "$FICHIER_SORTIE"
    cat "$FICHIER_STATS" >> "$FICHIER_SORTIE"
    
    echo "Fichier généré : $FICHIER_SORTIE"
    
    # Petit message informatif si l'usine n'a pas été trouvée (-1)
    if grep -q "\-1$" "$FICHIER_SORTIE"; then
        echo "Attention : Usine introuvable ou aucune donnée (-1)."
    fi

else
    echo "Erreur : Commande '$COMMANDE' inconnue."
    fin_script 1
fi

# Nettoyage final des fichiers temporaires
rm -f "$FICHIER_TMP" "$FICHIER_STATS"

# Fin normale avec affichage du temps
fin_script 0
