#include "main.h"  // Changement ici !

int main(int argc, char** argv) {
    // 1. Vérifications de base
    if (argc < 2) {
        return 1;
    }

    char* fichier_entree = argv[1];
    char* commande = argv[2];
    
    // ... Le reste de ton code reste identique ...
    // Juste une petite amélioration : utilise TAILLE_BUFFER au lieu de 1024 en dur
    
    // Exemple :
    // char ligne[TAILLE_BUFFER];
    // while (fgets(ligne, TAILLE_BUFFER, flux_entree) != NULL) { ... }

    // ... (Le reste du code que tu as déjà) ...
    
    return 0;
}
