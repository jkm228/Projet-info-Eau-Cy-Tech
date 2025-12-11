#!/bin/bash
# Le "hash-bang" pour spécifier l'interpréteur [cite: 921]

# --- VARIABLES [cite: 930] ---
# On capture le temps de début. 'date +%s' renvoie les secondes (commande exécutée [cite: 986])
DEBUT=$(date +%s)
EXECUTABLE="c-wire"
FICHIER_SORTIE="stats.csv"
FICHIER_LOG="c-wire.log"

# --- FONCTIONS  ---

# Fonction pour afficher l'aide si besoin
afficher_aide() {
    echo "Usage: $0 <fichier_csv> <commande> [option]"
    echo "Commandes:"
    echo "  histo max|src|real"
    echo "  leaks <identifiant>"
}

# Fonction de vérification des arguments
verifier_ordres() {
    # Vérification du nombre d'arguments ($#) 
    # On a besoin d'au moins 2 arguments (fichier + commande)
    if [ $# -lt 2 ]; then
        echo "Erreur : Arguments manquants."
        afficher_aide
        exit 1
    fi

    # Vérification si le fichier d'entrée existe (-f) 
    if [ ! -f "$1" ]; then
        echo "Erreur : Le fichier '$1' n'existe pas."
        exit 2
    fi

    # Structure "Selon Cas" pour vérifier la commande 
    case "$2" in
        "histo")
            # Pour histo, il faut 3 arguments (le 3ème est max, src ou real)
            if [ $# -ne 3 ]; then
                echo "Erreur : 'histo' nécessite une option (max, src, real)."
                exit 3
            fi
            # On vérifie que le 3ème argument est valide
            if [ "$3" != "max" ] && [ "$3" != "src" ] && [ "$3" != "real" ]; then
                echo "Erreur : Option '$3' invalide."
                exit 3
            fi
            ;;
        
        "leaks")
            # Pour leaks, il faut 3 arguments (le 3ème est l'ID)
            if [ $# -ne 3 ]; then
                echo "Erreur : 'leaks' nécessite un identifiant."
                exit 4
            fi
            ;;
        
        *)
            # Cas par défaut (*) si la commande n'est pas reconnue [cite: 1396]
            echo "Erreur : Commande '$2' inconnue."
            afficher_aide
            exit 1
            ;;
    esac
}

# Fonction pour compiler le programme C
preparer_terrain() {
    # On teste si l'exécutable existe et s'il est exécutable (-x) 
    if [ ! -x "$EXECUTABLE" ]; then
        echo "Compilation en cours..."
        make
        # On vérifie le code retour de la commande make ($?) [cite: 1470]
        if [ $? -ne 0 ]; then
            echo "Erreur : La compilation a échoué."
            exit 1
        fi
    fi
}

# Fonction pour lancer le programme C
lancer_calcul() {
    # Nettoyage de l'ancien fichier de sortie s'il existe (-f)
    if [ -f "$FICHIER_SORTIE" ]; then
        rm "$FICHIER_SORTIE"
    fi

    # Exécution du programme C avec les arguments ($1, $2, $3) 
    ./"$EXECUTABLE" "$1" "$2" "$3"

    # Vérification du succès du programme C ($?)
    if [ $? -ne 0 ]; then
        echo "Erreur lors de l'exécution du programme C."
        exit 1
    fi
}

# Fonction pour générer les graphiques avec Gnuplot
creer_visuels() {
    # On ne fait ça que si la commande est "histo" (égalité de chaînes) [cite: 1272]
    if [ "$2" = "histo" ]; then
        
        # Vérification que le fichier de stats existe
        if [ ! -f "$FICHIER_SORTIE" ]; then
            echo "Erreur : Le fichier de résultats n'a pas été créé."
            exit 1
        fi

        # Tri et extraction des données (Commandes courantes [cite: 1487])
        # sort -t';' -k2,2n : Trie sur la 2ème colonne (numérique)
        sort -t';' -k2,2n "$FICHIER_SORTIE" > data_triee.tmp

        # head : récupère les premières lignes [cite: 1479]
        head -n 5 data_triee.tmp > data_min.dat
        # tail : récupère les dernières lignes [cite: 1479]
        tail -n 5 data_triee.tmp > data_max.dat

        # Appel à Gnuplot via redirection d'entrée (Here-Doc) 
        gnuplot <<- EOF
            set terminal png size 1000,600
            set output 'graph_min.png'
            set title "Top 5 Min - $3"
            set style data histograms
            set style fill solid
            set datafile separator ";"
            plot "data_min.dat" using 2:xtic(1) title "Volume"

            set output 'graph_max.png'
            set title "Top 5 Max - $3"
            plot "data_max.dat" using 2:xtic(1) title "Volume"
EOF
        
        # Suppression des fichiers temporaires (rm) [cite: 1483]
        rm data_triee.tmp data_min.dat data_max.dat
    fi
}

# Fonction pour afficher le temps
afficher_temps() {
    FIN=$(date +%s)
    # Calcul arithmétique sur des entiers $((...)) 
    DUREE=$((FIN - DEBUT))
    echo "Durée totale : $DUREE secondes"
}

# --- EXÉCUTION DU SCRIPT ---
# Appel des fonctions dans l'ordre
# "$@" représente tous les arguments passés au script 

verifier_ordres "$@"
preparer_terrain
lancer_calcul "$@"
creer_visuels "$@"
afficher_temps
