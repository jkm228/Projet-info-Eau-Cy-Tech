#include <stdio.h>
#include <stdlib.h>
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
        // Col 3 (Index 2) : Identifiant Station
        // Col 4 (Index 3) : Capacité ou Volume Transporté (Input)
        // Col 5 (Index 4) : Consommation (Output)
        
        long val3 = chaineVersLong(cols[2]); // Souvent ID ou Capacité
        long val4 = chaineVersLong(cols[3]); // Volume 1 (Transport/Capacité)
        long val5 = chaineVersLong(cols[4]); // Volume 2 (Consommation)

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

            if (estEgal(mode, "real") || estEgal(mode, "lv")) {
                // MODE REAL : On ne regarde QUE la consommation finale (Col 5)
                // On ignore la Col 4 car elle contient les fuites potentielles en amont
                debit = val5;
            }
            else if (estEgal(mode, "src")) {
                // MODE SRC : On veut le volume TOTAL (incluant fuites)
                // On priorise la Col 4 (Volume entrant/transporté) qui est > Col 5
                if (val4 > 0) {
                    debit = val4;
                } else {
                    debit = val5;
                }
            }

            // Insertion dans l'arbre (sur l'ID de la station en Col 3 usually)
            if (debit > 0 && estEgal(cols[2], "-") == 0) {
                *racine = inserer(*racine, 0, cols[2], 0, debit);
            }
        }
    }

    fclose(fp);
}
