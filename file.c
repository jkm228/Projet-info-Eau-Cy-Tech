#include <stdio.h>
#include <stdlib.h>
#include "avl.h"
#include "file.h"

#define MAX_LIGNE 2048

// --- FONCTIONS UTILITAIRES (Respect des cours) ---

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
    // Gestion du cas vide, du tiret ou des espaces
    if (s[0] == '\0' || s[0] == '-' || s[0] == ' ') return 0;
    
    while (s[i] >= '0' && s[i] <= '9') {
        res = res * 10 + (s[i] - '0');
        i++;
    }
    return res;
}

// --- FONCTION DE CHARGEMENT ---

void charger(char* chemin, pStation* racine, char* mode) {
    FILE* fp = fopen(chemin, "r");
    if (fp == NULL) {
        printf("Erreur : impossible d'ouvrir le fichier %s\n", chemin);
        exit(1);
    }

    char ligne[MAX_LIGNE];
    
    // On saute l'en-tête (Ligne 1)
    fgets(ligne, MAX_LIGNE, fp);

    while (fgets(ligne, MAX_LIGNE, fp) != NULL) {
        
        // v0 = 4 colonnes strictement
        char cols[4][50]; 
        char tampon[50];
        
        int idxLigne = 0;
        int idxCol = 0;
        int idxTampon = 0;

        // Réinitialisation du tampon de colonnes
        for(int k=0; k<4; k++) cols[k][0] = '\0';

        // --- PARSING MANUEL (Séparateur ;) ---
        while (ligne[idxLigne] != '\0' && idxCol < 4) {
            char c = ligne[idxLigne];

            if (c == ';' || c == '\n' || c == '\r') {
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

        // --- LECTURE DES VALEURS (Spécifique v0) ---
        // Col 0 : Station Source
        // Col 1 : Station Destinataire
        // Col 2 : Capacité (Lien entre stations)
        // Col 3 : Consommation (Lien vers client)
        
        long valCapacite = chaineVersLong(cols[2]);
        long valConso    = chaineVersLong(cols[3]);

        // --- LOGIQUE DE TRI ---

        // 1. MAX : On s'intéresse aux stations (HVB, Plants) et leur capacité
        // Dans le v0, si Capacité > 0, c'est un tuyau entre stations.
        if (estEgal(mode, "max")) {
            if (valCapacite > 0) {
                // On stocke la destination (cols[1]) car c'est elle qui reçoit/traite
                *racine = inserer(*racine, 0, cols[1], valCapacite, 0);
            }
        }

        // 2. SRC : On s'intéresse aux usines (Plants)
        // On veut voir les flux principaux.
        else if (estEgal(mode, "src")) {
            if (valCapacite > 0) {
                 // On prend cols[1] (la station qui reçoit)
                *racine = inserer(*racine, 0, cols[1], 0, valCapacite);
            }
        }

        // 3. REAL : On s'intéresse aux clients
        // Si Conso > 0, c'est un client qui tire sur une station.
        else if (estEgal(mode, "real") || estEgal(mode, "lv")) {
            if (valConso > 0) {
                // On attribue la charge à la station Source (cols[0]) qui fournit le client
                *racine = inserer(*racine, 0, cols[0], 0, valConso);
            }
        }
    }
    fclose(fp);
}
