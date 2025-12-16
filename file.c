#include <stdio.h>
#include <stdlib.h>
#include "avl.h"
#include "file.h"

// Prototype local
double chaineVersDouble(char* s); // Si tu l'as déjà, garde la

void charger(char* chemin, pStation* racine) {
    FILE* f = fopen(chemin, "r");
    if (f == NULL) exit(2);

    char buf1[256]; // ID Station 1
    char buf2[256]; // ID Station 2
    double val1 = 0.0;
    double val2 = 0.0;

    // Le Shell va maintenant envoyer 2 types de fichiers :
    // 1. HISTO : ID;CAP;CONSO
    // 2. LEAKS : ID_SRC;ID_DEST;CAP;FUITE

    // Astuce : On lit 4 colonnes. Si le fichier n'en a que 3, val2 vaudra 0 ou lecture échouera partiellement.
    // On adapte le fscanf pour lire ligne par ligne
    char ligne[1024];
    while (fgets(ligne, 1024, f)) {
        int n = sscanf(ligne, "%[^;];%[^;];%lf;%lf", buf1, buf2, &val1, &val2);
        
        if (n == 3) { 
            // MODE HISTO (3 colonnes : ID;CAP;CONSO)
            // buf1=ID, buf2 (lu comme cap string -> à convertir si besoin, mais ici fscanf a pris %[^;])
            // ATTENTION : sscanf est piégeux. Simplifions.
            // On va supposer que le Shell envoie un format stricte.
            
            // Re-parsing propre :
            double c, k;
            sscanf(ligne, "%[^;];%lf;%lf", buf1, &c, &k);
            *racine = inserer(*racine, buf1, c, k);
        }
        else if (n == 4) {
            // MODE LEAKS (4 colonnes : SOURCE;DEST;VOL;FUITE)
            // On a besoin de l'AVL pour trouver les noeuds
            
            // 1. On s'assure que la Source existe
            *racine = inserer(*racine, buf1, 0, 0); 
            pStation src = rechercher(*racine, buf1);
            
            // 2. On s'assure que la Dest existe
            *racine = inserer(*racine, buf2, 0, 0);
            pStation dest = rechercher(*racine, buf2);
            
            // 3. On enregistre le volume entrant dans la dest (pour le calcul)
            // Note : val1 est le volume qui sort de src. 
            // Dans ce modèle simplifié, on stocke le volume direct dans le noeud pour l'instant
            // Mais pour leaks, on a juste besoin de la topologie et des % de fuites
            
            // Ajout connexion
            ajouterConnexion(src, dest, val2);
            
            // IMPORTANT : On stocke le volume initial dans la destination ? 
            // Non, le volume coule. On stockera le volume injecté au départ.
            // Si c'est une ligne 'Source -> Usine', val1 est le volume.
            dest->conso += val1; // Astuce : on cumule le volume reçu
        }
    }
    fclose(f);
}
