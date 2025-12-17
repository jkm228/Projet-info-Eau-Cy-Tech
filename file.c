#include <stdio.h>
#include <stdlib.h>
#include "avl.h"
#include "file.h"

void charger(char* chemin, pStation* racine) {
    FILE* f = fopen(chemin, "r");
    if (f == NULL) exit(2);

    char buf1[256];
    char buf2[256];
    double val1 = 0.0;
    double val2 = 0.0;
    char ligne[1024];

    // Lecture ligne par ligne
    while (fgets(ligne, 1024, f)) {
        // Tentative de lecture de 4 champs (Mode graphe complet)
        int n = sscanf(ligne, "%[^;];%[^;];%lf;%lf", buf1, buf2, &val1, &val2);
        
        if (n == 3) { 
            // Format Histo : ID;CAP;CONSO
            // Re-lecture pour être sûr des types
            double c, k;
            sscanf(ligne, "%[^;];%lf;%lf", buf1, &c, &k);
            *racine = inserer(*racine, buf1, c, k);
        }
        else if (n == 4) {
            // Format Leaks : SOURCE;DEST;VOL;FUITE
            
            // Insertion/Recherche des noeuds source et destination
            *racine = inserer(*racine, buf1, 0, 0); 
            pStation src = rechercher(*racine, buf1);
            
            *racine = inserer(*racine, buf2, 0, 0);
            pStation dest = rechercher(*racine, buf2);
            
            // Création du lien dans le graphe
            ajouterConnexion(src, dest, val2);
            
            // Ajout du volume initial à la source (si applicable)
            // Dans le fichier filtré, val1 est le volume source
            if (val1 > 0) src->conso += val1; 
        }
    }
    fclose(f);
}
