#include <stdio.h>
#include <stdlib.h>
#include "avl.h"
#include "file.h"

// Charge le fichier temporaire généré par le Shell
void charger(char* chemin, pStation* racine) {
    FILE* f = fopen(chemin, "r");
    if (f == NULL) {
        fprintf(stderr, "Erreur : Impossible d'ouvrir le fichier %s\n", chemin);
        exit(2);
    }

    char id_buffer[256];
    double cap = 0.0;
    double conso = 0.0;

    // Le format attendu est strict : STRING ; DOUBLE ; DOUBLE
    // %[^;] lit jusqu'au point-virgule
    while (fscanf(f, "%[^;];%lf;%lf\n", id_buffer, &cap, &conso) == 3) {
        // On insère directement dans l'arbre
        *racine = inserer(*racine, id_buffer, cap, conso);
    }

    fclose(f);
}
