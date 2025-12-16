#include <stdio.h>
#include <stdlib.h>
#include "avl.h"
#include "file.h"

// Fonction récursive de parcours du réseau (Arbre N-aire)
double calculerFuitesAval(pStation s) {
    if (s == NULL) return 0;
    
    double fuites_ici = 0;
    Tuyau* t = s->liste_aval;
    
    while (t != NULL) {
        // Le volume qui passe dans ce tuyau est le volume présent dans la station *source
        // (Simplification : on suppose que s->conso contient le volume d'eau qui traverse s)
        
        // Calcul fuite sur ce tronçon
        double perte = s->conso * (t->fuite_percent / 100.0);
        
        // On propage l'eau restante à l'enfant
        t->destinataire->conso += (s->conso - perte);
        
        // La fuite totale = fuite ce tuyau + fuites en aval
        fuites_ici += perte + calculerFuitesAval(t->destinataire);
        
        t = t->suivant;
    }
    return fuites_ici;
}

int main(int argc, char** argv) {
    if (argc < 2) return 1;

    pStation arbre = NULL;
    char* fichier_in = argv[1];
    char* target_id = (argc >= 3) ? argv[2] : NULL;

    // Chargement (Le file.c doit gérer le format 4 colonnes si mode leaks)
    charger(fichier_in, &arbre);

    FILE* f_out = fopen("stats.csv", "w");
    if (f_out == NULL) { liberer(arbre); return 1; }

    if (target_id != NULL) {
        // --- MODE LEAKS ---
        pStation s = rechercher(arbre, target_id);
        if (s != NULL) {
            // On lance le calcul récursif sur le réseau aval
            double total_fuites = calculerFuitesAval(s);
            fprintf(f_out, "%s;%.6f\n", target_id, total_fuites);
        } else {
            fprintf(f_out, "%s;-1\n", target_id);
        }
    } else {
        // --- MODE HISTO ---
        fprintf(f_out, "Station;Capacite;Consommation\n");
        infixe(arbre, f_out);
    }

    fclose(f_out);
    liberer(arbre);
    return 0;
}
