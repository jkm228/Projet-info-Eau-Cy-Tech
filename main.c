#include <stdio.h>
#include <stdlib.h>
#include "avl.h"
#include "file.h"

int main(int argc, char** argv) {
    // Usage 1 : ./c-wire <fichier_tmp> (Mode Histo - Affiche tout)
    // Usage 2 : ./c-wire <fichier_tmp> <Station_ID> (Mode Leaks - Cherche ID)
    
    if (argc < 2) {
        return 1;
    }

    pStation arbre = NULL;
    char* fichier_in = argv[1];
    char* target_id = NULL;

    if (argc >= 3) {
        target_id = argv[2]; // On cherche cet ID spécifique
    }

    // 1. Chargement
    charger(fichier_in, &arbre);

    // 2. Ecriture résultats
    FILE* f_out = fopen("stats.csv", "w");
    if (f_out == NULL) {
        liberer(arbre);
        return 1;
    }

    if (target_id != NULL) {
        // --- MODE LEAKS ---
        pStation s = rechercher(arbre, target_id);
        if (s != NULL) {
            // Trouvé : On affiche la consommation (utilisée pour stocker les fuites)
            fprintf(f_out, "%s;%.6f\n", s->id_str, s->conso);
        } else {
            // Pas trouvé : Consigne = afficher ID avec volume -1
            fprintf(f_out, "%s;-1\n", target_id);
        }
    } else {
        // --- MODE HISTO ---
        // En-tête générique pour que le Shell puisse trier
        fprintf(f_out, "Station;Capacite;Consommation\n");
        infixe(arbre, f_out);
    }

    fclose(f_out);
    liberer(arbre);

    return 0;
}
