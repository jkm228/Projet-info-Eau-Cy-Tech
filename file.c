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
    // On ignore la première ligne (en-têtes) si nécessaire
    // fgets(buffer, TAILLE_BUFFER, fichier);

    while (fgets(buffer, TAILLE_BUFFER, fichier) != NULL) {
        char id_origine[50] = "";
        char id_dest[50] = "";
        long capacite = 0;
        long debit = 0;
        char* token;

        // --- Parsing des 4 colonnes ---
        // Format supposé : ID_A;ID_B;CAPACITE;DEBIT (ou LOAD)

        // Colonne 1 : ID Origine (Source)
        token = strtok(buffer, ";");
        if (token != NULL && strcmp(token, "-") != 0) {
            strncpy(id_origine, token, 49);
            id_origine[49] = '\0';
        }

        // Colonne 2 : ID Destination (Station/Usine)
        token = strtok(NULL, ";");
        if (token != NULL && strcmp(token, "-") != 0) {
            strncpy(id_dest, token, 49);
            id_dest[49] = '\0';
        }

        // Colonne 3 : Capacité
        token = strtok(NULL, ";");
        if (token != NULL && strcmp(token, "-") != 0) {
            capacite = atol(token);
        }

        // Colonne 4 : Débit (Load)
        token = strtok(NULL, ";\n"); // \n pour la fin de ligne
        if (token != NULL && strcmp(token, "-") != 0) {
            debit = atol(token);
        }

        // --- Logique d'insertion selon l'option ---
        
        // CAS 1 : Option MAX (On veut la capacité de la station)
        if (strcmp(option, "max") == 0) {
            // Si on a un ID Destination et une Capacité, c'est la définition de la station
            if (strlen(id_dest) > 0 && capacite > 0) {
                *racine = insererStation(*racine, 0, id_dest, capacite, 0);
            }
            // Parfois la station est définie dans la col 1 (cas rares), sécurité :
            else if (strlen(id_origine) > 0 && capacite > 0 && strlen(id_dest) == 0) {
                *racine = insererStation(*racine, 0, id_origine, capacite, 0);
            }
        }
        
        // CAS 2 : Option SRC ou REAL (On veut cumuler le volume arrivant)
        else if (strcmp(option, "src") == 0 || strcmp(option, "real") == 0) {
            // Pour ces modes, on s'intéresse à ce qui arrive dans la station (Destination)
            // On ajoute le 'debit' au volume traité de la station 'id_dest'
            if (strlen(id_dest) > 0 && debit > 0) {
                *racine = insererStation(*racine, 0, id_dest, 0, debit);
            }
        }
    }

    fclose(fichier);
}
