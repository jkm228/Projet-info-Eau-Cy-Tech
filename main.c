#include <stdio.h>
#include <stdlib.h>
#include "wildwater.h"
#include "file.h"

int main(int argc, char** argv) {
    // Vérification rapide des arguments
    if (argc < 4) {
        fprintf(stderr, "Usage: %s <fichier> <commande> <option>\n", argv[0]);
        return 1;
    }

    // 1. Initialisation
    Station* racine = NULL;
    char* chemin_entree = argv[1]; // Le fichier .dat
    char* option = argv[3];        // "max", "min", etc.

    // 2. Chargement des données (C'est file.c qui fait le travail)
    chargerDonnees(chemin_entree, &racine, option);

    // 3. Écriture du fichier de sortie
    FILE* flux_sortie = fopen("stats.csv", "w");
    if (flux_sortie == NULL) {
        perror("Erreur lors de la création de stats.csv");
        libererAVL(racine);
        return 1;
    }

    // On écrit l'arbre trié dans le fichier
    parcoursInfixe(racine, flux_sortie);

    // 4. Nettoyage et fermeture
    fclose(flux_sortie);
    libererAVL(racine);

    return 0;
}
