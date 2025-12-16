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
        
        // CORRECTION : On passe à 5 colonnes pour correspondre à ton fichier v0
        char cols[5][50]; 
        char tampon[50];
        
        int idxLigne = 0;
        int idxCol = 0;
        int idxTampon = 0;

        for(int k=0; k<5; k++) cols[k][0] = '\0';

        // --- PARSING MANUEL (idxCol < 5) ---
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

        // --- RECUPERATION VALEURS (Adapté à ton fichier v0 à 5 colonnes) ---
        // Col 0: Ignoré ou ID global
        // Col 1: Source / Service
        // Col 2: Destinataire / Usine / Client
        // Col 3: Capacité (Indices décalés de +1 par rapport à avant)
        // Col 4: Consommation
        
        long valCapacite = chaineVersLong(cols[3]);     // Était cols[2]
        long valConsommation = chaineVersLong(cols[4]); // Était cols[3]

        // --- LOGIQUE DISTINCTE ---

        // MODE MAX : Capacité
        if (estEgal(mode, "max")) {
            if (valCapacite > 0) {
                // La station concernée est en Col 2 (ex: Plant #JA...)
                *racine = inserer(*racine, 0, cols[2], valCapacite, 0);
            }
        }

        // MODE SRC : Volume Capté
        else if (estEgal(mode, "src")) {
            if (valCapacite > 0) {
                // On garde Col 2 (Usine qui reçoit) pour que le grep "Plant" fonctionne
                *racine = inserer(*racine, 0, cols[2], 0, valCapacite);
            }
        }

        // MODE REAL : Volume Traité (Consommation Client)
        else if (estEgal(mode, "real") || estEgal(mode, "lv")) {
            if (valConsommation > 0) {
                // Le client est en Col 2, mais c'est la station en Col 1 (Service) qui fournit
                *racine = inserer(*racine, 0, cols[1], 0, valConsommation);
            }
        }
    }
    fclose(fp);
}
