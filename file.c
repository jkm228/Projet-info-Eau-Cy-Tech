#include <stdio.h>
#include <stdlib.h>
#include "avl.h"
#include "file.h"

#define MAX_LIGNE 2048

// --- FONCTIONS UTILITAIRES ---

int estEgal(char* s1, char* s2) {
    int i = 0;
    while (s1[i] != '\0' && s2[i] != '\0') {
        if (s1[i] != s2[i]) return 0;
        i++;
    }
    return (s1[i] == '\0' && s2[i] == '\0');
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
    if (s[0] == '\0' || s[0] == '-') return 0;
    
    while (s[i] >= '0' && s[i] <= '9') {
        res = res * 10 + (s[i] - '0');
        i++;
    }
    return res;
}

// --- FONCTION PRINCIPALE ---

void charger(char* chemin, pStation* racine, char* mode) {
    FILE* fp = fopen(chemin, "r");
    if (fp == NULL) {
        printf("Erreur : impossible d'ouvrir le fichier %s\n", chemin);
        exit(1);
    }

    char ligne[MAX_LIGNE];
    
    fgets(ligne, MAX_LIGNE, fp); // On saute l'en-tête

    while (fgets(ligne, MAX_LIGNE, fp) != NULL) {
        
        char cols[5][50]; 
        char tampon[50];
        
        int idxLigne = 0;
        int idxCol = 0;
        int idxTampon = 0;

        for(int k=0; k<5; k++) cols[k][0] = '\0';

        // --- PARSING MANUEL (5 colonnes) ---
        while (ligne[idxLigne] != '\0' && idxCol < 5) {
            char c = ligne[idxLigne];

            if (c == ';' || c == '\t' || c == '\n' || c == '\r') {
                tampon[idxTampon] = '\0';
                if (idxTampon == 0) copierChaine(cols[idxCol], "-");
                else copierChaine(cols[idxCol], tampon);
                idxCol++;
                idxTampon = 0;
            } 
            else {
                tampon[idxTampon] = c;
                idxTampon++;
            }
            idxLigne++;
        }

        // --- LOGIQUE HYBRIDE (C'est ici que tout se joue) ---

        // Cas 1 : Ligne commençant par "-" => C'est un TUYAU (Source -> Usine)
        // Structure : - ; Source ; Usine(Dest) ; Capacité ; ...
        if (estEgal(cols[0], "-")) {
            long valCapacite = chaineVersLong(cols[3]); // Col 4
            
            // MODE MAX et SRC utilisent ces lignes
            if (valCapacite > 0) {
                if (estEgal(mode, "max") || estEgal(mode, "src")) {
                    // On insère l'Usine Destinataire (cols[2])
                    *racine = inserer(*racine, 0, cols[2], valCapacite, 0);
                }
            }
        }
        
        // Cas 2 : Ligne ne commençant PAS par "-" => C'est un CLIENT (Usine -> Client)
        // Structure : Usine(Source) ; Service ; Client ; - ; Consommation
        else {
            long valConso = chaineVersLong(cols[4]); // Col 5

            // MODE REAL utilise ces lignes
            if (valConso > 0) {
                if (estEgal(mode, "real") || estEgal(mode, "lv")) {
                    // On insère l'Usine Fournisseur (cols[0])
                    *racine = inserer(*racine, 0, cols[0], 0, valConso);
                }
            }
        }
    }
    fclose(fp);
}
