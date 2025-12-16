#ifndef AVL_H
#define AVL_H

#include <stdio.h>
#include <stdlib.h>

// Structure Station modifiée pour gérer les réels
typedef struct Station {
    // int id; // Supprimé car les IDs sont textuels (ex: "Plant #JA...")
    char id_str[50];        // Identifiant (Clé de l'arbre)
    double capacite;        // Capacité (double pour gérer les virgules)
    double conso;           // Consommation
    int h;                  // Hauteur
    struct Station *fg;     // Fils gauche
    struct Station *fd;     // Fils droit
} Station;

typedef Station* pStation;

// Fonctions de base
int max(int a, int b);
int hauteur(pStation a);
int equilibre(pStation a);

// Fonctions de rotation
pStation rotationDroite(pStation y);
pStation rotationGauche(pStation x);
pStation doubleRotationGD(pStation a);
pStation doubleRotationDG(pStation a);

// Fonctions principales
// Note : on passe des double pour cap et flux
pStation creerStation(char* code, double cap);
pStation inserer(pStation a, char* code, double cap, double flux);
void infixe(pStation a, FILE* fs); 
void liberer(pStation a);

#endif
