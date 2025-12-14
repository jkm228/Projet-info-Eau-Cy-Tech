#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wildwater.h"
#include "file.h"

#define TAILLE_BUFFER 2048

void chargerDonnees(const char* chemin_fichier, Station** racine, const char* option) {
    FILE* fichier = fopen(chemin_fichier, "r");
    if (fichier == NULL) {
        perror("Erreur ouverture fichier");
        exit(1);
    }

    char buffer[TAILLE_BUFFER];
    // On passe l'en-tête
    fgets(buffer, TAILLE_BUFFER, fichier);

    const char* separateurs = ";\r\n";

    while (fgets(buffer, TAILLE_BUFFER, fichier) != NULL) {
        char col1[50] = "-";
        char col2[50] = "-";
        char col3[50] = "-";
        char col4[50] = "-";
        char col5[50] = "-"; 
        char* token;

        // --- LECTURE ROBUSTE DES 5 COLONNES ---
        token = strtok(buffer, separateurs);
        if (token) strncpy(col1, token, 49);

        token = strtok(NULL, separateurs);
        if (token) strncpy(col2, token, 49);

        token = strtok(NULL, separateurs);
        if (token) strncpy(col3, token, 49);

        token = strtok(NULL, separateurs);
        if (token) strncpy(col4, token, 49);
        
        token = strtok(NULL, separateurs);
        if (token) strncpy(col5, token, 49);

        // --- CONVERSION (Float pour gérer "1.381") ---
        // On utilise atof pour lire les virgules, puis on convertit en long
        long val3 = atol(col3); 
        long val4 = (long)atof(col4); // Capacity (parfois float ?)
        long val5 = (long)atof(col5); // Débit (souvent float type 1.381)

        // --- LOGIQUE ---
        if (strcmp(option, "max") == 0) {
            // Pour MAX, on prend la capacité (souvent Col 4, parfois Col 3)
            // On ignore le débit actuel (Col 5)
            long capacite = (val4 > 0) ? val4 : val3;
            
            if (capacite > 0) {
                if (strcmp(col2, "-") != 0 && strcmp(col3, "-") == 0) {
                    *racine = insererStation(*racine, 0, col2, capacite, 0);
                }
                else if (strcmp(col1, "-") != 0 && strcmp(col2, "-") == 0) {
                    *racine = insererStation(*racine, 0, col1, capacite, 0);
                }
            }
        }
        else if (strcmp(option, "src") == 0 || strcmp(option, "real") == 0) {
            // Pour SRC/REAL, le débit est en Col 5 (ex: 1.381).
            // Si Col 5 est vide/nul, on regarde Col 4 par sécurité.
            long debit = (val5 > 0) ? val5 : val4;
            
            // On accumule le débit sur la Destination (Col 3)
            if (debit > 0 && strcmp(col3, "-") != 0) {
                *racine = insererStation(*racine, 0, col3, 0, debit);
            }
        }
    }

    fclose(fichier);
}
