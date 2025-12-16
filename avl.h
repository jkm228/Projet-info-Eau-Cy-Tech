#ifndef AVL_H
#define AVL_H

#include <stdio.h>
#include <stdlib.h>

// Structure pour la liste chaînée des enfants (Arbre N-aire pour les fuites)
typedef struct Tuyau {
    struct Station* destinataire; // Pointeur vers le noeud enfant
    double fuite_percent;         // % de fuite sur ce tuyau
    struct Tuyau* suivant;        // Prochain tuyau partant de la même station
} Tuyau;

// Structure Station (Noeud AVL)
typedef struct Station {
    char id_str[50];        // Clé (Identifiant)
    double capacite;        // Pour Histo : Capacité
    double conso;           // Pour Histo : Consommation / Pour Leaks : Volume traversant
    
    // NOUVEAU : Tête de la liste chaînée des enfants
    Tuyau* liste_aval;      
    
    int h;                  // Hauteur AVL
    struct Station *fg;     // Fils gauche AVL
    struct Station *fd;     // Fils droit AVL
} Station;

typedef Station* pStation;

// Fonctions de base
int max(int a, int b);
int hauteur(pStation a);
int equilibre(pStation a);

// Rotations
pStation rotationDroite(pStation y);
pStation rotationGauche(pStation x);
pStation doubleRotationGD(pStation a);
pStation doubleRotationDG(pStation a);

// Fonctions principales
pStation creerStation(char* code, double cap);
pStation inserer(pStation a, char* code, double cap, double flux);
pStation rechercher(pStation a, char* code); // Indispensable pour leaks
void liberer(pStation a);
void infixe(pStation a, FILE* fs);

// NOUVEAU : Fonction pour connecter deux stations (Graphe)
void ajouterConnexion(pStation parent, pStation enfant, double fuite);

#endif
