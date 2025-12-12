#include "wildwater.h"

int main(int argc, char** argv) {
    // 1. Vérifications de base (Le script le fait déjà, mais c'est une sécurité)
    if (argc < 2) {
        return 1;
    }

    char* fichier_entree = argv[1];
    char* commande = argv[2];
    char* option = "";
    
    if (argc >= 4) {
        option = argv[3];
    }

    // 2. Ouverture des fichiers
    FILE* flux_entree = fopen(fichier_entree, "r");
    if (flux_entree == NULL) {
        printf("Erreur lecture fichier\n");
        return 1;
    }

    FILE* flux_sortie = fopen("stats.csv", "w");
    if (flux_sortie == NULL) {
        printf("Erreur écriture fichier\n");
        fclose(flux_entree);
        return 1;
    }

    // 3. Variables pour lire le CSV
    char ligne[1024];
    Station* racine = NULL;

    // On ignore la première ligne (en-tête) si besoin, sinon on attaque direct
    // fgets lit le fichier ligne par ligne
    while (fgets(ligne, 1024, flux_entree) != NULL) {
        // Parsing simple avec strtok ou sscanf
        // Format attendu: ID_Station;ID_Consommateur;Capacite;...
        
        int id_station = 0;
        long capacite = 0;
        long consommation = 0;
        
        // --- NOTE IMPORTANTE ---
        // Ici, tu dois adapter le parsing (découpage de la ligne) 
        // exactement selon les colonnes de TON fichier CSV.
        // Exemple simplifié :
        char* token = strtok(ligne, ";"); 
        if(token != NULL) id_station = atoi(token);
        
        token = strtok(NULL, ";"); // on passe à la colonne suivante
        token = strtok(NULL, ";"); // Colonne capacité ?
        if(token != NULL && strcmp(token, "-") != 0) capacite = atol(token);

        // 4. Logique de remplissage selon la commande
        // Si on fait "histo", on remplit l'AVL
        if (strcmp(commande, "histo") == 0) {
            if (id_station > 0) {
                // On insère dans l'arbre
                racine = insererStation(racine, id_station, "Station", 0, capacite);
            }
        }
    }

    // 5. Écriture du résultat et nettoyage
    parcoursInfixe(racine, flux_sortie);
    
    libererAVL(racine);
    fclose(flux_entree);
    fclose(flux_sortie);

    return 0;
}
