#include <stdio.h>
#include <stdlib.h>
#include "avl.h"
#include "file.h"

#define MAX_LIGNE 2048

// --- FONCTIONS UTILITAIRES MANUELLES (Pas de <string.h>) ---

// Remplace strcmp : compare deux chaînes caractère par caractère
int estEgal(char* s1, char* s2) {
    int i = 0;
    while (s1[i] != '\0' && s2[i] != '\0') {
        if (s1[i] != s2[i]) {
            return 0; // Différent
        }
        i++;
    }
    // On vérifie qu'elles finissent en même temps
    if (s1[i] == '\0' && s2[i] == '\0'){
        return 1; // Identique
    }
    return 0;
}

// Remplace strcpy : copie src dans dest caractère par caractère
void copierChaine(char* dest, const char* src) {
    int i = 0;
    while (src[i] != '\0') {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0'; // N'oublie pas le caractère de fin
}

// Convertit une chaîne en entier long (gère le tiret "-")
long chaineVersLong(char* s) {
    long res = 0;
    int i = 0;
    
    // Si vide ou tiret, ça vaut 0
    if (s[0] == '\0' || s[0] == '-'){
        return 0;
    }
    // Tant qu'on a des chiffres
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
    
    // On lit et ignore la première ligne (l'en-tête)
    fgets(ligne, MAX_LIGNE, fp);

    // Lecture ligne par ligne
    while (fgets(ligne, MAX_LIGNE, fp) != NULL) {
        
        // Structure pour le fichier v0 (4 colonnes attendues)
        char cols[4][50]; 
        char tampon[50];  
        
        int idxLigne = 0; 
        int idxCol = 0;   
        int idxTampon = 0;

        // On vide les colonnes avant de commencer
        for(int k=0; k<4; k++) cols[k][0] = '\0';

        // --- PARSING MANUEL (Boucle sur les caractères) ---
        while (ligne[idxLigne] != '\0' && idxCol < 4) {
            char c = ligne[idxLigne];

            // Si séparateur ou fin de ligne
            if (c == ';' || c == '\n' || c == '\r') {
                tampon[idxTampon] = '\0'; // Finir le mot
                
                if (idxTampon == 0) {
                    copierChaine(cols[idxCol], "-");
                } else {
                    copierChaine(cols[idxCol], tampon);
                }

                idxCol++;      // Colonne suivante
                idxTampon = 0; // Reset du tampon
            } 
            else {
                tampon[idxTampon] = c; // On remplit le tampon
                idxTampon++;
            }
            idxLigne++;
        }
        
        // --- INTERPRÉTATION DES COLONNES (Format v0) ---
        // cols[0] : Station Départ (Source)
        // cols[1] : Destinataire (Station ou Client)
        // cols[2] : Capacité (Lien Station-Station)
        // cols[3] : Consommation (Client final)
        
        long valCapacite = chaineVersLong(cols[2]);    // Col 3
        long valConsommation = chaineVersLong(cols[3]); // Col 4

        //
