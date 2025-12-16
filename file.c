#include <stdio.h>
#include <stdlib.h>
#include <string.h> // Nécessaire pour strstr
#include "avl.h"
#include "file.h"

#define MAX_LIGNE 2048

int estEgal(char* s1, char* s2) {
    int i = 0;
    while (s1[i] != '\0' && s2[i] != '\0') {
        if (s1[i] != s2[i]) {
            return 0; 
        }
        i++;
    }
    if (s1[i] == '\0' && s2[i] == '\0'){
        return 1;
    }
    return 0;
}

void copierChaine(char* dest, const char* src) {
    int i = 0;
    while (src[i] != '\0') {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
}

long chaineVersLong(char* s) {
    long res = 0;
    int i = 0;
    
    if (s[0] == '\0' || s[0] == '-'){
        return 0;
    }
    while (s[i] >= '0' && s[i] <= '9') {
        res = res * 10 + (s[i] - '0');
        i++;
    }
    return res;
}

// Fonction pour deviner si l'ID est un Client (Entreprise/Individu) ou une Station
// Les Stations contiennent généralement "HVB", "HVA", "LV" ou sont vides
int estClient(char* id) {
    // Si l'ID contient ces mots-clés, c'est une Station ou une Source (PAS un client)
    if (strstr(id, "HVB") != NULL || strstr(id, "HVA") != NULL || strstr(id, "Source") != NULL) {
        return 0; 
    }
    // Idem pour LV si tes stations s'appellent LV
    if (strstr(id, "LV") != NULL) {
        return 0;
    }
    // Si ce n'est pas une station, on considère que c'est un client (Comp, Indiv, etc.)
    return 1;
}

void charger(char* chemin, pStation* racine, char* mode) {
    FILE* fp = fopen(chemin, "r");
    if (fp == NULL) {
        printf("Erreur : impossible d'ouvrir le fichier %s\n", chemin);
        exit(1);
    }

    char ligne[MAX_LIGNE];
    
    // On ignore l'en-tête
    fgets(ligne, MAX_LIGNE, fp);

    while (fgets(ligne, MAX_LIGNE, fp) != NULL) {
        
        char cols[5][50]; 
        char tampon[50];  
        
        int idxLigne = 0; 
        int idxCol = 0;   
        int idxTampon = 0;

        for(int k=0; k<5; k++) cols[k][0] = '\0';

        while (ligne[idxLigne] != '\0' && idxCol < 5) {
            char c = ligne[idxLigne];

            if (c == ';' || c == '\n' || c == '\r') {
                tampon[idxTampon] = '\0';
                
                if (idxTampon == 0) {
                    copierChaine(cols[idxCol], "-");
                } else {
                    copierChaine(cols[idxCol], tampon);
                }

                idxCol++;
                idxTampon = 0;
            } 
            else {
                tampon[idxTampon] = c;
                idxTampon++;
            }
            idxLigne++;
        }
        
        // --- Récupération des valeurs ---
        // Cols[1] : ID du partenaire (Source, Autre Station ou Client)
        // Cols[2] : ID de la Station concernée (celle qu'on veut grapher)
        // Cols[3] : Volume 1 (Souvent Capacité ou Flux Principal)
        // Cols[4] : Volume 2 (Souvent Consommation Individuelle)
        
        long val3 = chaineVersLong(cols[2]); 
        long val4 = chaineVersLong(cols[3]); 
        long val5 = chaineVersLong(cols[4]); 

        // --- Logique d'insertion ---

        // 1. MODE MAX (Capacité)
        if (estEgal(mode, "max")) {
            long cap = 0;
            if (val4 > 0) cap = val4;
            else cap = val3;

            if (cap > 0) {
                if (estEgal(cols[1], "-") == 0) { 
                    *racine = inserer(*racine, 0, cols[1], cap, 0);
                }
                else if (estEgal(cols[0], "-") == 0) { 
                    *racine = inserer(*racine, 0, cols[0], cap, 0);
                }
            }
        }
        // 2. MODES SRC et REAL (Consommation / Flux)
        else {
            long debit = 0;
            // On prend la valeur disponible (val4 prioritaire car souvent plus complète)
            long valeurLigne = (val4 > 0) ? val4 : val5;

            // ANALYSE DU TYPE DE PARTENAIRE (Cols[1])
            int partenaireEstClient = estClient(cols[1]);

            if (estEgal(mode, "real") || estEgal(mode, "lv")) {
                // MODE REAL : On ne veut que les CONSOMMATEURS
                if (partenaireEstClient == 1) {
                    debit = valeurLigne;
                }
            }
            else if (estEgal(mode, "src")) {
                // MODE SRC : On veut les SOURCES et AUTRES STATIONS (Flux Entrant)
                // Donc on prend tout ce qui N'EST PAS un client final
                if (partenaireEstClient == 0) {
                    debit = valeurLigne;
                }
            }

            // Insertion (uniquement si on a trouvé un débit pertinent)
            if (debit > 0 && estEgal(cols[2], "-") == 0) {
                *racine = inserer(*racine, 0, cols[2], 0, debit);
            }
        }
    }

    fclose(fp);
}
