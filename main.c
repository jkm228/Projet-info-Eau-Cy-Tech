#include "main.h"

int main(int argc, char** argv) {
    if (argc < 4) { return 1; }

    char* chemin_entree = argv[1];
    char* commande = argv[2];
    char* option = argv[3];

    FILE* flux_entree = ouvrir_fichier(chemin_entree, "r");
    FILE* flux_sortie = ouvrir_fichier("stats.csv", "w");

    Station* racine = NULL;

    // Buffer pour l'ID (Texte)
    char id_str[50];
    long cap = 0;

    // On passe id_str à lire_ligne_csv
    while (lire_ligne_csv(flux_entree, id_str, &cap)) {
        
        if (strcmp(commande, "histo") == 0) {
             // On ignore les lignes vides ou sans capacité
            if (strlen(id_str) > 0 && cap > 0) {
                // On insère avec l'ID TEXTE. 
                // On met 0 pour l'ID numérique car il ne sert plus au tri.
                racine = insererStation(racine, 0, id_str, 0, cap);
            }
        }
    }

    parcoursInfixe(racine, flux_sortie);
    
    libererAVL(racine);
    fermer_fichier(flux_entree);
    fermer_fichier(flux_sortie);

    return 0;
}
