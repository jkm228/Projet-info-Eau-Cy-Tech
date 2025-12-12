#include "main.h"

int main(int argc, char** argv) {
    // 1. Vérification des arguments
    if (argc < 4) {
        printf("Usage: %s <fichier> <commande> <option>\n", argv[0]);
        return 1;
    }

    char* chemin_entree = argv[1];
    char* commande = argv[2];
    char* option = argv[3];

    // 2. Ouverture des fichiers via file.c
    FILE* flux_entree = ouvrir_fichier(chemin_entree, "r");
    FILE* flux_sortie = ouvrir_fichier("stats.csv", "w");

    // 3. Initialisation AVL
    Station* racine = NULL;

    // Variables tampons pour la lecture
    int id = 0;
    long cap = 0;
    long deb = 0;

    // 4. Boucle principale de lecture
    // On utilise la fonction de file.c qui gère le parsing
    while (lire_ligne_csv(flux_entree, &id, &cap, &deb)) {
        
        // Logique "Histo"
        if (strcmp(commande, "histo") == 0) {
            // Si on cherche le MAX (Capacité)
            if (strcmp(option, "max") == 0) {
                if (id > 0 && cap > 0) {
                    racine = insererStation(racine, id, "Station", 0, cap);
                }
            }
            // Autres options (src, real...) à adapter ici
        }
    }

    // 5. Écriture et Nettoyage
    parcoursInfixe(racine, flux_sortie);
    
    libererAVL(racine);
    fermer_fichier(flux_entree);
    fermer_fichier(flux_sortie);

    return 0;
}
