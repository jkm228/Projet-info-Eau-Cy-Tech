#include <stdio.h>
#include <stdlib.h>
#include "avl.h"
#include "file.h"

// Fonction récursive de parcours du réseau (Arbre N-aire)
double calculerFuitesAval(pStation s) {
    if (s == NULL) return 0;
    
    // 1. On compte le nombre d'enfants (tuyaux sortants)
    int nb_enfants = 0;
    Tuyau* t = s->liste_aval;
    while (t != NULL) {
        nb_enfants++;
        t = t->suivant;
    }

    // Si c'est un cul-de-sac (client final), pas de fuite aval
    if (nb_enfants == 0) return 0;

    // 2. Principe de conservation : On divise le volume par le nombre de tuyaux
    // (Hypothèse de répartition équitable faute de données précises)
    double volume_par_enfant = s->conso / nb_enfants;
    
    double fuites_totales_ici = 0;
    t = s->liste_aval; // On reprend au début de la liste
    
    while (t != NULL) {
        // Calcul de la fuite sur CE tuyau spécifique
        // On applique le % de fuite sur la part de volume qui passe ici
        double perte_tuyau = volume_par_enfant * (t->fuite_percent / 100.0);
        
        // On transmet l'eau restante à la station suivante
        if (t->destinataire != NULL) {
            t->destinataire->conso += (volume_par_enfant - perte_tuyau);
            
            // On ajoute la fuite du tuyau + les fuites des enfants de l'enfant (récursif)
            fuites_totales_ici += perte_tuyau + calculerFuitesAval(t->destinataire);
        }
        
        t = t->suivant;
    }
    
    return fuites_totales_ici;
}

int main(int argc, char** argv) {
    // Usage : ./c-wire <fichier_tmp> [Target_ID]
    
    if (argc < 2) return 1;

    pStation arbre = NULL;
    char* fichier_in = argv[1];
    char* target_id = (argc >= 3) ? argv[2] : NULL;

    // Chargement
    charger(fichier_in, &arbre);

    FILE* f_out = fopen("stats.csv", "w");
    if (f_out == NULL) {
        liberer(arbre);
        return 1;
    }

    if (target_id != NULL) {
        // --- MODE LEAKS ---
        pStation s = rechercher(arbre, target_id);
        if (s != NULL) {
            // Le calcul récursif va maintenant diviser les flux correctement
            double total_fuites = calculerFuitesAval(s);
            fprintf(f_out, "%s;%.6f\n", target_id, total_fuites);
        } else {
            // Station introuvable
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
