#!/bin/bash

# ===================================================================
# PROJET C-WILDWATER - Script de Traitement
# ===================================================================

# 1. GESTION DE L'AIDE (-h)
if [ "$1" = "-h" ]; then
    echo "Usage: $0 <fichier_dat> <type_station> <type_conso> [id_station]"
    echo ""
    echo "Types de station :"
    echo "  usine     : Analyse des usines de traitement (Plant)"
    echo "  stockage  : Analyse des réservoirs (Storage)"
    echo "  source    : Analyse des sources (Source/Well)"
    echo ""
    echo "Types de consommateur :"
    echo "  all       : Tous les consommateurs"
    echo "  ind       : Individuels"
    echo "  ent       : Entreprises"
    echo ""
    echo "Exemple :"
    echo "  $0 c-wildwater_v0.dat usine all"
    exit 0
fi

# 2. VERIFICATION DES ARGUMENTS
if [ $# -lt 3 ]; then
    echo "Erreur : Nombre d'arguments insuffisant."
    echo "Utilisez '$0 -h' pour l'aide."
    exit 1
fi

FICHIER_DAT="$1"
TYPE_STATION="$2"
TYPE_CONSO="$3"
# ID_STATION="$4" (Optionnel, pour plus tard)

# Vérification de l'existence du fichier
if [ ! -f "$FICHIER_DAT" ]; then
    echo "Erreur : Le fichier '$FICHIER_DAT' est introuvable."
    exit 2
fi

# 3. NETTOYAGE ET COMPILATION
echo "Nettoyage et compilation du projet C..."
if [ -f "Makefile" ]; then
    make clean > /dev/null
    make > /dev/null
    if [ $? -ne 0 ]; then
        echo "Erreur : La compilation a échoué."
        exit 3
    fi
else
    echo "Erreur : Makefile introuvable."
    exit 3
fi

# 4. FILTRAGE DES DONNÉES (L'étape clé !)
# On prépare un fichier temporaire simple pour le C : "ID;CAPACITE;CONSOMMATION"
# Cela évite de gérer les cas bizarres dans le C.

FICHIER_TMP="input_data.tmp"
echo "Filtrage des données pour le type '$TYPE_STATION'..."

# Définition du motif de recherche selon le type demandé
# (On adapte les mots-clés du sujet aux mots-clés du fichier .dat)
case "$TYPE_STATION" in
    "usine")
        MOTIF="Plant"
        ;;
    "stockage")
        MOTIF="Storage"
        ;;
    "source")
        MOTIF="Source|Well" # Regex pour prendre les deux
        ;;
    *)
        echo "Erreur : Type de station '$TYPE_STATION' inconnu."
        exit 1
        ;;
esac

# Utilisation de AWK pour normaliser les données
# Si la ligne définit la station (ex: contient "Plant" et une Capacité) -> ID;CAP;0
# Si la ligne est un flux (ex: "Plant" vers "Client") -> ID;0;CONSO
awk -F';' -v motif="$MOTIF" '
BEGIN { OFS=";" }
# Cas 1 : Définition de la station (Capacité en col 4, pas de tiret)
$2 ~ motif && $4 != "-" {
    print $2, $4, 0
}
# Cas 2 : Flux sortant de la station (Station en col 1, Conso en col 5)
$1 ~ motif && $5 != "-" {
    print $1, 0, $5
}
' "$FICHIER_DAT" > "$FICHIER_TMP"

# Vérification que le filtrage a donné quelque chose
if [ ! -s "$FICHIER_TMP" ]; then
    echo "Attention : Aucune donnée trouvée pour ce type de station."
    exit 0
fi

# 5. EXECUTION DU PROGRAMME C
# Le programme C prendra le fichier temporaire, et le mode (si besoin)
echo "Lancement du traitement C..."
./c-wire "$FICHIER_TMP" "$TYPE_STATION" "$TYPE_CONSO"

if [ $? -ne 0 ]; then
    echo "Erreur lors de l'exécution du programme C."
    exit 4
fi

# 6. TRI FINAL ET AFFICHAGE (Optionnel ici, souvent fait par le C ou après)
# Le sujet demande souvent de trier la sortie.
# Supposons que le C a produit "output.csv".
if [ -f "stats.csv" ]; then
    echo "Tri des résultats..."
    # Exemple de tri : Par capacité décroissante (col 2)
    sort -t';' -k2,2nr stats.csv > "resultats_${TYPE_STATION}.csv"
    echo "Terminé. Résultats dans 'resultats_${TYPE_STATION}.csv'."
    
    # Nettoyage
    rm "$FICHIER_TMP"
else
    echo "Erreur : Le fichier de sortie 'stats.csv' n'a pas été généré."
fi

# Fin du script
