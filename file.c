#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "avl.h"
#include "file.h"

#define MAX_LIGNE 2048

void charger(char* chemin, pStation* racine, char* mode) {
    FILE* fp = fopen(chemin, "r");
    if (fp == NULL) {
        fprintf(stderr, "Erreur : impossible d'ouvrir le fichier %s\n", chemin);
        exit(1);
    }

    char ligne[MAX_LIGNE];
    // On ignore la première ligne (entête)
    fgets(ligne, MAX_LIGNE, fp);

    while (fgets(ligne, MAX_LIGNE, fp) != NULL) {
        char cols[5][50]; // Tableau pour stocker les 5 colonnes
        int i = 0;
        
        // Découpage de la ligne avec strtok
        char* token = strtok(ligne, ";\r\n");
        while (token != NULL && i < 5) {
            strncpy(cols[i], token, 49);
            cols[i][49] = '\0'; // Sécurité
            token = strtok(NULL, ";\r\n");
            i++;
        }
        
        // Remplissage des colonnes manquantes si ligne incomplète
        while (i < 5) {
            strcpy(cols[i], "-");
            i++;
        }

        // Conversion des valeurs
        // Note: atof gère les nombres à virgule (1.381) qu'on caste en long
        long val3 = atol(cols[2]); 
        long val4 = (long)atof(cols[3]); 
        long val5 = (long)atof(cols[4]); 

        // Logique d'insertion selon le mode
        if (strcmp(mode, "max") == 0) {
            long cap = (val4 > 0) ? val4 : val3;
            if (cap > 0) {
                // Station en col 2 ou 3 ?
                if (strcmp(cols[1], "-") != 0) {
                    *racine = inserer(*racine, 0, cols[1], cap, 0);
                }
                else if (strcmp(cols[0], "-") != 0) {
                    *racine = inserer(*racine, 0, cols[0], cap, 0);
                }
            }
        }
        else {
            // Mode src ou real : on regarde le débit (col 5 ou 4)
            long debit = (val5 > 0) ? val5 : val4;
            if (debit > 0 && strcmp(cols[2], "-") != 0) {
                *racine = inserer(*racine, 0, cols[2], 0, debit);
            }
        }
    }

    fclose(fp);
}
