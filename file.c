#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "wildwater.h"
#include "file.h"

void chargerDonnees(const char* chemin_fichier, Station** racine, const char* option) {
    FILE* fichier = fopen(chemin_fichier, "r");

    if (fichier == NULL) {
        fprintf(stderr, "Erreur d'ouverture du fichier %s : %s\n", chemin_fichier, strerror(errno));
        exit(1);
    }

    char buffer[1024];

    fgets(buffer, 1024, fichier);

    while (fgets(buffer, 1024, fichier) != NULL) {
        
        int id_a = 0;
        int id_b = 0;
        long capacite = 0;
        long debit = 0;

        char* token = strtok(buffer, ";");
        if (token != NULL && strcmp(token, "-") != 0) {
            id_a = atoi(token);
        }

        token = strtok(NULL, ";");
        if (token != NULL && strcmp(token, "-") != 0) {
            id_b = atoi(token);
        }

        token = strtok(NULL, ";");
        if (token != NULL && strcmp(token, "-") != 0) {
            if (strchr(token, '-') == NULL) {
                 capacite = atol(token);
            }
        }

        token = strtok(NULL, ";");
        if (token != NULL && strcmp(token, "-") != 0) {
             if (strchr(token, '-') == NULL) {
                debit = atol(token);
             }
        }

        if (strcmp(option, "max") == 0) {
            if (id_a != 0 && capacite > 0) {
                *racine = insererStation(*racine, id_a, "Station", 0, capacite);
            }
        }
        else {
            if (id_a != 0 && id_b != 0 && debit > 0) {
                 *racine = insererStation(*racine, id_b, "Client", 0, debit);
            }
            else if (id_a != 0 && debit > 0) {
                 *racine = insererStation(*racine, id_a, "Producteur", 0, debit);
            }
        }
    }

    fclose(fichier);
}
