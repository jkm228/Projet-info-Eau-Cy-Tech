#include <stdio.h>
#include <stdlib.h>
#include "avl.h"
#include "file.h"

int main(int argc, char** argv) {
    if (argc < 4) {
        printf("Usage: %s <fichier> <commande> <mode>\n", argv[0]);
        return 1;
    }

    pStation arbre = NULL;
    char* fichier_in = argv[1];
    char* mode = argv[3];

    // Chargement
    charger(fichier_in, &arbre, mode);

    // Ecriture résultats
    FILE* f_out = fopen("stats.csv", "w");
    if (f_out == NULL) {
        liberer(arbre);
        return 1;
    }

    // Parcours pour remplir le fichier trié
    infixe(arbre, f_out);

    fclose(f_out);
    liberer(arbre);

    return 0;
}
