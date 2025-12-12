#!/bin/bash
# Le "hash-bang" pour spécifier l'interpréteur

# --- VARIABLES ---
DEBUT=$(date +%s)
EXECUTABLE="c-wire"
FICHIER_SORTIE="stats.csv"

# --- FONCTIONS ---

afficher_aide() {
    echo "Usage: $0 <fichier_csv> <commande> [option]"
    echo "Commandes:"
    echo "  histo max|src|real"
    echo "  leaks <identifiant>"
}

verifier_ordres() {
    # On a besoin d'au moins 2 arguments
    if [ $# -lt 2 ]; then
        echo "Erreur : Arguments manquants."
        afficher_aide
        exit 1
    fi

    if [ ! -f "$1" ]; then
        echo "Erreur : Le fichier '$1' n'existe pas."
        exit 2
    fi

    case "$2" in
        "histo")
            if [ $# -ne 3 ]; then
                echo "Erreur : 'histo' nécessite une option (max, src, real)."
                exit 3
            fi
            if [ "$3" != "max" ] && [ "$3" != "src" ] && [ "$3" != "real" ]; then
                echo "Erreur : Option '$3' invalide."
                exit 3
            fi
            ;;
        
        "leaks")
            if [ $# -ne 3 ]; then
                echo "Erreur : 'leaks' nécessite un identifiant."
                exit 4
            fi
            ;;
        
        *)
            echo "Erreur : Commande '$2' inconnue."
            afficher_aide
            exit 1
            ;;
    esac
}

preparer_terrain() {
    if [ ! -x "$EXECUTABLE" ]; then
        echo "Compilation en cours..."
        make
        if [ $? -ne 0 ]; then
            echo "Erreur : La compilation a échoué."
            exit 1
        fi
    fi
}

lancer_calcul() {
    if [ -f "$FICHIER_SORTIE" ]; then
        rm "$FICHIER_SORTIE"
    fi

    # Lancement du C
    ./"$EXECUTABLE" "$1" "$2" "$3"

    if [ $? -ne 0 ]; then
        echo "Erreur lors de l'exécution du programme C."
        exit 1
    fi
}

creer_visuels() {
    # On génère le graphique seulement pour la commande "histo"
    if [ "$2" = "histo" ]; then
        
        if [ ! -f "$FICHIER_SORTIE" ]; then
            echo "Erreur : Le fichier de résultats n'a pas été créé."
            exit 1
        fi

        # --- CORRECTION ICI : Choix de la colonne ---
        # Si option = max, on regarde la colonne 2 (Capacité)
        # Sinon (src/real), on regarde la colonne 3 (Volume)
        if [ "$3" = "max" ]; then
            COL=2
        else
            COL=3
        fi

        echo "Génération des graphiques sur la colonne $COL..."

        # Tri numérique sur la bonne colonne (-k$COL)
        sort -t';' -k${COL},${COL}n "$FICHIER_SORTIE" > data_triee.tmp

        # Extraction des 5 premiers et 5 derniers
        head -n 5 data_triee.tmp > data_min.dat
        tail -n 5 data_triee.tmp > data_max.dat

        # Appel Gnuplot avec la colonne dynamique
        gnuplot <<- EOF
            set terminal png size 1000,600
            set output 'graph_min.png'
            set title "Top 5 Min - $3"
            set style data histograms
            set style fill solid
            set datafile separator ";"
            set ylabel "Quantité"
            set xlabel "ID Station"
            # On plot la colonne COL en Y, et la colonne 1 (ID) en étiquette X
            plot "data_min.dat" using ${COL}:xtic(1) title "$3"

            set output 'graph_max.png'
            set title "Top 5 Max - $3"
            plot "data_max.dat" using ${COL}:xtic(1) title "$3"
EOF
        
        # Nettoyage
        rm data_triee.tmp data_min.dat data_max.dat
    fi
}

afficher_temps() {
    FIN=$(date +%s)
    DUREE=$((FIN - DEBUT))
    echo "Durée totale : $DUREE secondes"
}

# --- EXÉCUTION ---

verifier_ordres "$@"
preparer_terrain
lancer_calcul "$@"
creer_visuels "$@"
afficher_temps
