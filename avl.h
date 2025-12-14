#ifndef AVL_H
#define AVL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// --- STRUCTURES ---

// Structure Station (Nœud AVL)
typedef struct Station {
    int id_numerique;
    char id_str[50];
    int capacite;
    long volume_traite;       // long pour éviter l'overflow
    struct Station *fils_gauche;
    struct Station *fils_droit;
    int hauteur;              // Pour l'équilibrage AVL
} Station;

// Structure Connexion (Liste Chaînée pour les fuites)
typedef struct Connexion {
    struct Station* station_fils;
    float pourcentage_fuite;
    struct Connexion* suivant;
} Connexion;

// --- PROTOTYPES AVL (Implémentés dans avl.c) ---

// Utilitaires
int max(int a, int b);
int hauteur(Station *N);
int facteurEquilibre(Station *N);

// Rotations
Station* rotationDroite(Station *y);
Station* rotationGauche(Station *x);
Station* doubleRotationGaucheDroite(Station *z);
Station* doubleRotationDroiteGauche(Station *z);

// Gestion Arbre
Station* creerStation(int id, char* id_str, int capacite);
Station* insererStation(Station* noeud, int id, char* id_str, int capacite, int volume_ajout);

// --- LA LIGNE CORRIGÉE EST ICI ---
Station* rechercherStation(Station* racine, int id, char* id_str);
// --------------------------------

void parcoursInfixe(Station *racine, FILE* flux_sortie);
void libererAVL(Station* noeud);

#endif
