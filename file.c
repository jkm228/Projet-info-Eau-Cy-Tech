#include <stdio.h>
#include <stdlib.h>
#include "avl.h"
#include "file.h"

#define MAX_LIGNE 2048

// --- Fonctions utilitaires manuelles (remplacent string.h et stdlib.h) ---

// Remplace strcmp : Renvoie 1 si les chaines sont identiques, 0 sinon
int estEgal(const char* s1, const char* s2) {
    int i = 0;
    while (s1[i] != '\0' && s2[i] != '\0') {
        if (s1[i] != s2[i]) {
            return 0; // Différent
        }
        i++;
    }
    // Si les deux finissent en même temps, c'est égal
    if (s1[i] == '\0' && s2[i] == '\0') return 1;
    return 0;
}

// Remplace strcpy : Copie src dans dest
void copierChaine(char* dest, const char* src) {
    int i = 0;
    while (src[i] != '\0') {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0'; // Ne pas oublier le caractère de fin
}

// Remplace atol/atof : Convertit une chaine en entier long
// S'arrête si on croise un point (pour gérer les 45.0 qui deviennent 45)
long chaineVersLong(const char* s) {
    long res = 0;
    int i = 0;
    
    // Gestion du cas vide ou tiret
    if (s[0] == '\0' || s[0] == '-') return 0;

    while (s[i] >= '0' && s[i] <= '9') {
        res = res * 10 + (s[i] - '0'); // Conversion ASCII vers entier
        i++;
    }
    return res;
}

// --- Fonction principale ---

void charger(char* chemin, pStation* racine, char* mode) {
    FILE* fp = fopen(chemin, "r");
    if (fp == NULL) {
        printf("Erreur : impossible d'ouvrir le fichier %s\n", chemin);
        exit(1);
    }

    char ligne[MAX_LIGNE];
    
    // Lire la première ligne (entête) pour l'ignorer
    fgets(ligne, MAX_LIGNE, fp);

    // Lecture ligne par ligne
    while (fgets(ligne, MAX_LIGNE, fp) != NULL) {
        
        char cols[5][50]; // Tableau pour stocker les 5 colonnes
        char tampon[50];  // Tampon temporaire pour lire un mot
        
        int idxLigne = 0; // Position dans la ligne brute
        int idxCol = 0;   // Quelle colonne on remplit (0 à 4)
        int idxTampon = 0;// Position dans le mot en cours

        // Initialisation des colonnes à vide par sécurité
        for(int k=0; k<5; k++) cols[k][0] = '\0';

        // --- PARSING MANUEL (Remplace strtok) ---
        // On parcourt la ligne caractère par caractère
        while (ligne[idxLigne] != '\0' && idxCol < 5) {
            char c = ligne[idxLigne];

            // Si on tombe sur un séparateur ou la fin de ligne
            if (c == ';' || c == '\n' || c == '\r') {
                tampon[idxTampon] = '\0'; // Finir la chaine
                
                // Si le tampon est vide, on met un tiret "-" pour dire "rien"
                if (idxTampon == 0) {
                    copierChaine(cols[idxCol], "-");
                } else {
                    copierChaine(cols[idxCol], tampon);
                }

                idxCol++;      // On passe à la colonne suivante
                idxTampon = 0; // On vide le tampon
            } 
            else {
                // Sinon, on ajoute le caractère au mot en cours
                tampon[idxTampon] = c;
                idxTampon++;
            }
            idxLigne++;
        }
        // ----------------------------------------

        // Conversion des valeurs numériques avec notre fonction
        long val3 = chaineVersLong(cols[2]);
        long val4 = chaineVersLong(cols[3]);
        long val5 = chaineVersLong(cols[4]);

        // Logique d'insertion (identique à avant, mais avec estEgal)
        if (estEgal(mode, "max")) {
            // On cherche la capacité (col 3 ou 4)
            long cap = 0;
            if (val4 > 0) cap = val4;
            else cap = val3;

            if (cap > 0) {
                // Est-ce une station normale ou HVA ?
                if (estEgal(cols[1], "-") == 0) { // Si col 1 n'est pas vide
                    *racine = inserer(*racine, 0, cols[1], cap, 0);
                }
                else if (estEgal(cols[0], "-") == 0) { // Si col 0 n'est pas vide
                    *racine = inserer(*racine, 0, cols[0], cap, 0);
                }
            }
        }
        else {
            // Mode src ou real (consommation)
            long debit = 0;
            if (val5 > 0) debit = val5;
            else debit = val4;

            // On ne prend que si débit > 0 et qu'il y a un ID consommateur (col 2)
            if (debit > 0 && estEgal(cols[2], "-") == 0) {
                *racine = inserer(*racine, 0, cols[2], 0, debit);
            }
        }
    }

    fclose(fp);
}
