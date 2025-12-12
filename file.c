#include "file.h"

FILE* ouvrir_fichier(char* chemin, char* mode) {
    FILE* f = fopen(chemin, mode);
    if (f == NULL) {
        fprintf(stderr, "Erreur critique : Impossible d'ouvrir le fichier %s\n", chemin);
        exit(2);
    }
    return f;
}

void fermer_fichier(FILE* f) {
    if (f != NULL) fclose(f);
}

// Nouvelle logique de parsing adaptée à TON fichier
int lire_ligne_csv(FILE* f, char* id_station, long* capacite) {
    char buffer[TAILLE_LIGNE];
    char* token;

    // Initialisation
    strcpy(id_station, ""); 
    *capacite = 0;

    if (fgets(buffer, TAILLE_LIGNE, f) == NULL) return 0;

    // Col 1 : Amont (souvent "-" pour les sources) - On ignore
    token = strtok(buffer, ";");

    // Col 2 : ID STATION (ex: "Source #XL900000Q") - C'est celui qu'on veut !
    token = strtok(NULL, ";");
    if (token != NULL) {
        strcpy(id_station, token);
    }

    // Col 3 : Destination - On ignore
    token = strtok(NULL, ";");

    // Col 4 : CAPACITÉ (ex: "517") - C'est le volume !
    token = strtok(NULL, ";");
    if (token != NULL && strcmp(token, "-") != 0) {
        *capacite = atol(token);
    }

    // Col 5 : Fuite - On ignore pour l'instant
    
    return 1;
}
