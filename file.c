#include "file.h"

// Ouvre un fichier avec vérification d'erreur
FILE* ouvrir_fichier(char* chemin, char* mode) {
    FILE* f = fopen(chemin, mode);
    if (f == NULL) {
        fprintf(stderr, "Erreur critique : Impossible d'ouvrir le fichier %s\n", chemin);
        exit(2); // Code erreur 2 pour problème fichier
    }
    return f;
}

void fermer_fichier(FILE* f) {
    if (f != NULL) {
        fclose(f);
    }
}

// Fonction de parsing robuste
// Elle lit une ligne brute et remplit les variables passées par adresse
int lire_ligne_csv(FILE* f, int* id_station, long* capacite, long* debit) {
    char buffer[TAILLE_LIGNE];
    char* token;

    // Initialisation des valeurs par défaut
    *id_station = 0;
    *capacite = 0;
    *debit = 0;

    // Lecture de la ligne brute
    if (fgets(buffer, TAILLE_LIGNE, f) == NULL) {
        return 0; // Fin de fichier
    }

    // Découpage (Parsing) avec strtok
    // Format supposé: ID_Station;ID_Dest;Capacite;Debit;...
    
    // 1. ID Station (Col 1)
    token = strtok(buffer, ";");
    if (token != NULL && strcmp(token, "-") != 0) {
        *id_station = atoi(token);
    }

    // 2. ID Destination (Col 2) - On passe (token suivant)
    token = strtok(NULL, ";");
    
    // 3. Capacité (Col 3)
    token = strtok(NULL, ";");
    if (token != NULL && strcmp(token, "-") != 0) {
        *capacite = atol(token);
    }
    
    // 4. Débit/Conso (Col 4) - Selon ton fichier, adapte si c'est Col 3 ou 4
    // Ici on suppose que le débit peut être dans la colonne suivante si c'est un tuyau
    /* token = strtok(NULL, ";"); 
       if (token != NULL && strcmp(token, "-") != 0) *debit = atol(token); */

    return 1; // Succès
}
