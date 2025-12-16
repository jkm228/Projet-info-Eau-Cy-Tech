#include <stdio.h>
#include <stdlib.h>
#include "avl.h"
#include "file.h"

#define MAX_LIGNE 2048

// --- FONCTIONS UTILITAIRES (Style Cours - Pas de string.h) ---

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
    
    // On saute l'en-tête
    fgets(ligne, MAX_LIGNE, fp);

    while (fgets(ligne, MAX_LIGNE, fp) != NULL) {
        
        char cols[4][50]; // v0 a 4 colonnes
        char tampon[50];
        
        int idxLigne = 0;
        int idxCol = 0;
        int idxTampon = 0;

        // Init colonnes
        for(int k=0; k<4; k++) cols[k][0] = '\0';

        // --- PARSING MANUEL ---
        while (ligne[idxLigne] != '\0' && idxCol < 4) {
            char c = ligne[idxLigne];

            // Séparateurs : Point-virgule, Tabulation, Retour ligne
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

        // --- RECUPERATION VALEURS (v0) ---
        // Col 0: Source ID
        // Col 1: Dest ID / Usine
        // Col 2: Capacité (Tuyau) -> Indice 2
        // Col 3: Consommation (Client) -> Indice 3
        
        long valCapacite = chaineVersLong(cols[2]);
        long valConsommation = chaineVersLong(cols[3]);

        // --- LOGIQUE DISTINCTE ---

        // MODE MAX : Capacité des Usines
        if (estEgal(mode, "max")) {
            if (valCapacite > 0 && valConsommation == 0) {
                // cols[1] est l'usine concernée
                *racine = inserer(*racine, 0, cols[1], valCapacite, 0);
            }
        }

        // MODE SRC : Volume Capté par les Usines
        else if (estEgal(mode, "src")) {
            if (valCapacite > 0 && valConsommation == 0) {
                // IMPORTANT : On insère l'USINE (cols[1]) qui reçoit l'eau
                // pour que le filtre "Plant" du script shell fonctionne.
                *racine = inserer(*racine, 0, cols[1], 0, valCapacite);
            }
        }

        // MODE REAL : Volume Traité (Consommation Client)
        else if (estEgal(mode, "real") || estEgal(mode, "lv")) {
            if (valConsommation > 0) {
                // On attribue la conso à l'usine qui dessert (cols[0])
                *racine = inserer(*racine, 0, cols[0], 0, valConsommation);
            }
        }
    }
    fclose(fp);
}
