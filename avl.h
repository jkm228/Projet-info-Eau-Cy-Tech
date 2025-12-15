#ifndef AVL_H
#define AVL_H

#include <stdio.h>
#include <stdlib.h>
// On a retiré <string.h> pour faire "maison"

// Définition de la structure conforme aux cours (fg/fd)
typedef struct Station {
    int id;                 // Identifiant numérique
    char id_str[50];        // Identifiant texte
    long capacite;          // Capacité (pour le max)
    long conso;             // Consommation (pour src/real)
    int h;                  // Hauteur du noeud
    struct Station *fg;     // Fils gauche
    struct Station *fd;     // Fils droit
} Station;

typedef Station* pStation; // Pointeur vers Station (style L2)

// Fonctions de base
int max(int a, int b);
int hauteur(pStation a);
int equilibre(pStation a); // Facteur d'équilibre

// Rotations
pStation rotationDroite(pStation y);
pStation rotationGauche(pStation x);
pStation doubleRotationGD(pStation a);
pStation doubleRotationDG(pStation a);

// Fonctions principales
pStation creerStation(int id, char* code, long cap);
pStation inserer(pStation a, int id, char* code, long cap, long flux);
void infixe(pStation a, FILE* fs); // Parcours infixe inverse
void liberer(pStation a);

#endif
