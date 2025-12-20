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
        //Lecture de 4 champs (Mode Leaks : SRC;DEST;VOL;FUITE)
        int n = sscanf(ligne, "%[^;];%[^;];%lf;%lf", buf1, buf2, &val1, &val2);
        
        if (n == 4) {
            // MODE LEAKS
            
            // 1. Insertion/Recherche des noeuds source et destination
            *racine = inserer(*racine, buf1, 0, 0); 
            pStation src = rechercher(*racine, buf1);
            
            *racine = inserer(*racine, buf2, 0, 0);
            pStation dest = rechercher(*racine, buf2);
            
            // 2. Création du lien physique dans le graphe
            ajouterConnexion(src, dest, val2);
            
            // 3. Gestion des flux d'eau 
            // Si la ligne contient un volume (ex: Source -> Plant), c'est de l'eau qui entre dans le système.
            // On l'ajoute à la station DESTINATAIRE (l'Usine) pour qu'elle puisse la redistribuer ensuite.
            if (val1 > 0) {
                dest->conso += val1; 
                // On l'ajoute aussi à la source au cas où on voudrait partir de la source
                src->conso += val1;  
            }
        }
        else {
            // MODE HISTO (3 colonnes : ID;CAP;CONSO)
        
            double c, k;
            if (sscanf(ligne, "%[^;];%lf;%lf", buf1, &c, &k) == 3) {
                *racine = inserer(*racine, buf1, c, k);
            }
        }
    }
    fclose(f);
}
