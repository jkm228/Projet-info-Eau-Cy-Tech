#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wildwater.h"
#include "file.h"

#define TAILLE_BUFFER 1024

void chargerDonnees(const char* chemin_fichier, Station** racine, const char* option) {
    FILE* fichier = fopen(chemin_fichier, "r");
    if (fichier == NULL) {
        perror("Erreur ouverture fichier");
        exit(1);
    }

    char buffer[TAILLE_BUFFER];
    // On ignore la ligne d'en-tête si elle existe
    // fgets(buffer, TAILLE_BUFFER, fichier); 

    while (fgets(buffer, TAILLE_BUFFER, fichier) != NULL) {
        char id_station[50] = "";
        long capacite = 0;
        char* token;

        // Le format de ton fichier est : -;ID_STATION;DEST;CAPACITE;...

        // 1. On ignore la première colonne (-)
        token = strtok(buffer, ";");

        // 2. On lit l'ID de la station (C'est du texte !)
        token = strtok(NULL, ";");
        if (token != NULL) {
            strncpy(id_station, token, 49);
            id_station[49] = '\0'; // Sécurité
        }

        // 3. On ignore la destination
        token = strtok(NULL, ";");

        // 4. On lit la capacité (C'est un chiffre)
        token = strtok(NULL, ";");
        if (token != NULL && strcmp(token, "-") != 0) {
            capacite = atol(token);
        }

        // Si on a bien un ID et une capacité, on insère
        if (strlen(id_station) > 0 && capacite > 0) {
            // On passe 0 en id numérique car on utilise le texte
            *racine = insererStation(*racine, 0, id_station, 0, capacite);
        }
    }

    fclose(fichier);
}
