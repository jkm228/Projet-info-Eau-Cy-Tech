#include <stdio.h>
#include <stdlib.h>
#include "avl.h"
#include "file.h"

int main(int argc, char** argv) {
    // Le programme C attend maintenant exactement 1 argument utile : le fichier d'entrée
    // Usage interne : ./c-wire <fichier_tmp>
    
    if (argc < 2) {
        printf("Usage interne: %s <fichier_tmp>\n", argv[0]);
        return 1;
    }

    pStation arbre = NULL;
    char* fichier_in = argv[1];

    // 1. Chargement des données (lecture du fichier temporaire)
    charger(fichier_in, &arbre);

    // 2. Ecriture des résultats
    FILE* f_out = fopen("stats.csv", "w");
    if (f_out == NULL) {
        fprintf(stderr, "Erreur d'écriture stats.csv\n");
        liberer(arbre);
        return 1;
    }

    // En-tête du CSV (optionnel mais propre)
    fprintf(f_out, "Station;Capacite;Consommation\n");
    
    // Parcours infixe (Tri alphabétique par ID)
    infixe(arbre, f_out);

    fclose(f_out);
    
    // 3. Libération de la mémoire
    liberer(arbre);

    return 0;
}
