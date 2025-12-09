
DEBUT=$(date +%s%3N)
verifier les ordres() {
	# 1. Vérification du nombre minimum d'arguments
	if [$# -lt 2 ]; then 
		echo "Erreur : Arguments manquants"
        	echo "Usage : $0 <fichier_donnees> <commande> [options]"
        	exit 1
    	fi
    	# 2. Vérification de l'existence du fichiers données
    	if [! -f "$1"]; then
    		echo "Erreur : le fichier de données '$1' est introuvable"
    		exit 2
    	fi
    	# 3. Vérification de la commande e de ses options
    	case "$2" in
        "histo")
            # Vérification pour la commande 'histo'
            # On attend exactement 3 arguments au total : fichier + histo + mode
            if [ $# -ne 3 ]; then
                echo "Erreur : La commande 'histo' attend exactement 1 argument (max, src ou real)"
                exit 3
            fi

            # On vérifie que le 3ème argument est valide
            if [ "$3" != "max" ] && [ "$3" != "src" ] && [ "$3" != "real" ]; then
                echo "Erreur : Mode '$3' inconnu pour histo. Utilisez 'max', 'src' ou 'real'"
                exit 3
            fi
            ;;

        "leaks")
            # Vérification pour la commande 'leaks'
            # On attend exactement 3 arguments au total : fichier + leaks + identifiant
            if [ $# -ne 3 ]; then
                echo "Erreur : La commande 'leaks' attend exactement 1 identifiant d'usine"
                exit 4
            fi
            
            # On vérifie que l'identifiant n'est pas vide (juste une sécurité)
            if [ -z "$3" ]; then
                echo "Erreur : L'identifiant est vide"
                exit 4
            fi
            ;;

        *)
            # Si le 2ème argument n'est ni 'histo' ni 'leaks'
            echo "Erreur : Commande '$2' inconnue. Les commandes valides sont 'histo' et 'leaks'"
            exit 1
            ;;
    esac
}
preparer_terrain() {
    # Nom de ton exécutable (tel que défini dans ton Makefile)
    # CHANGE LE NOM ICI si ton programme s'appelle autrement !
    EXECUTABLE="c-wire" 

    # 1. On vérifie si l'exécutable existe déjà
    # -f teste si le fichier existe
    # -x teste si le fichier est exécutable
    if [ ! -x "$EXECUTABLE" ]; then
        echo "L'exécutable '$EXECUTABLE' est absent."
        echo "Lancement de la compilation"
        
        # 2. On lance la compilation via make
        make

        # 3. On vérifie si la compilation a réussi
        # $? contient le code retour de la dernière commande (make)
        # 0 signifie succès, tout autre chiffre signifie erreur
        if [ $? -ne 0 ]; then
            echo "Erreur : La compilation a échoué"
            exit 1
        fi
        
        echo "Compilation terminée avec succès"
    else
        echo "L'exécutable existe déjà"
    fi
}
lancer_calcul() {
    # 1. On définit le nom de l'exécutable (doit être le même que dans 'preparer_terrain')
    EXECUTABLE="./c-wire"
    # 2. On lance le programme C en lui transmettant les arguments
    echo "Lancement du traitement C"
    $EXECUTABLE "$1" "$2" "$3"
    # 3. On vérifie comment le programme C s'est terminé
    if [ $? -ne 0 ]; then
        echo "Erreur : Le programme C a rencontré un problème"
        echo "Arrêt du script"
        exit 1
    else
        echo "Traitement C terminé avec succès"
    fi
}
creer_visuels() {
    # 1. On vérifie si on est bien en mode "histo"
    if [ "$2" != "histo" ]; then
        return
    fi

    echo "Génération des graphiques en cours..."

    FICHIER_DATA="stats.csv"
    # 2. Vérification que le fichier de données existe
    if [ ! -f "$FICHIER_DATA" ]; then
        echo "Erreur : Le fichier '$FICHIER_DATA' n'a pas été généré par le programme C."
        exit 1
    fi

    # 3. Préparation des données (Tri)
    sort -t';' -k2 -n "$FICHIER_DATA" > data_triee.tmp
    head -n 50 data_triee.tmp > data_min.dat
    tail -n 10 data_triee.tmp > data_max.dat

    # 4. Lancement de Gnuplot
    gnuplot <<- EOF
        # Configuration de l'image de sortie
        set terminal png size 1200,800
        
        # --- Graphique 1 : Les 50 plus petits ---
        set output 'graph_min.png'
        set title "Les 50 plus petites usines ($3)"
        set style data histograms
        set style fill solid
        set boxwidth 0.5
        set xtics rotate by -45 scale 0 font ",8"
        set ylabel "Volume"
        set grid
        # On trace en utilisant la colonne 2 pour la hauteur et la col 1 pour le nom (xtic)
        plot "data_min.dat" using 2:xtic(1) notitle linecolor rgb "blue"

        # --- Graphique 2 : Les 10 plus grands ---
        set output 'graph_max.png'
        set title "Les 10 plus grandes usines ($3)"
        plot "data_max.dat" using 2:xtic(1) notitle linecolor rgb "red"
EOF

    # 5. Nettoyage des fichiers temporaires
    rm data_triee.tmp data_min.dat data_max.dat

    echo "Graphiques 'graph_min.png' et 'graph_max.png' générés."
}
afficher_temps() {
    # 1. On capture l'heure de fin
    FIN=$(date +%s%3N)
    # 2. On calcule la différence
    DUREE=$((FIN - DEBUT))
    # 3. On affiche le résultat final
    echo "Durée totale du traitement : $DUREE ms"
}



# Appel des fonction
verifier_ordres "$@"
preparer_terrain
lancer_calcul "$@"
creer_visuels "$@"
afficher_temps
