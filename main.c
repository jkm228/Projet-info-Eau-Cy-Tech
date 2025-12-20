#include <stdio.h>
#include <stdlib.h>
#include "avl.h"
#include "file.h"

// Parcours récursif pour calculer les pertes
double calculerFuitesAval(pStation s) {
    if (s == NULL) return 0;
    
    // Compter le nombre de sorties
    int nb_enfants = 0;
    Tuyau* t = s->liste_aval;
    while (t != NULL) {
        nb_enfants++;
        t = t->suivant;
    }

    if (nb_enfants == 0) return 0;

    // Répartition équitable du volume qui sort
    double volume_par_enfant = s->conso / nb_enfants;
    double fuites_totales = 0;
    
    t = s->liste_aval;
    while (t != NULL) {
        // Fuite sur la zone actuel
        double perte_tuyau = volume_par_enfant * (t->fuite_percent / 100.0);
        
        if (t->destinataire != NULL) {
            // Transmission du volume restant
            t->destinataire->conso += (volume_par_enfant - perte_tuyau);
            
            // Appel récursif
            fuites_totales += perte_tuyau + calculerFuitesAval(t->destinataire);
        }
        
        t = t->suivant;
    }
    
    return fuites_totales;
}

int main(int argc, char** argv) {
    if (argc < 2) return 1;

    pStation arbre = NULL;
    char* fichier_in = argv[1];
    char* target_id = (argc >= 3) ? argv[2] : NULL;

    charger(fichier_in, &arbre);

    FILE* f_out = fopen("stats.csv", "w");
    if (f_out == NULL) {
        liberer(arbre);
        return 1;
    }

    if (target_id != NULL) {
        // Mode Leaks
        pStation s = rechercher(arbre, target_id);
        if (s != NULL) {
            double total = calculerFuitesAval(s);
            fprintf(f_out, "%s;%.6f\n", target_id, total);
        } else {
            fprintf(f_out, "%s;-1\n", target_id);
        }
    } else {
        // Mode Histo
        fprintf(f_out, "Station;Capacite;Consommation\n");
        infixe(arbre, f_out);
    }

    fclose(f_out);
    liberer(arbre);

    return 0;
}
