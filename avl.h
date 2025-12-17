#ifndef AVL_H
#define AVL_H

#include <stdio.h>
#include <stdlib.h>

// Liste chainée pour représenter les tuyaux sortants (enfants)
typedef struct Tuyau {
    struct Station* destinataire;
    double fuite_percent;
    struct Tuyau* suivant;
} Tuyau;

// Structure d'un noeud de l'AVL (Station)
typedef struct Station {
    char id_str[50];        // Identifiant unique
    double capacite;        // Capacité (Mode Histo)
    double conso;           // Volume d'eau (Mode Histo et Leaks)
    
    Tuyau* liste_aval;      // Liste des stations en aval
    
    int h;                  // Hauteur du noeud
    struct Station *fg;     // Fils gauche
    struct Station *fd;     // Fils droit
} Station;

typedef Station* pStation;

// Utilitaires AVL
int max(int a, int b);
int hauteur(pStation a);
int equilibre(pStation a);

// Rotations pour l'équilibrage
pStation rotationDroite(pStation y);
pStation rotationGauche(pStation x);
pStation doubleRotationGD(pStation a);
pStation doubleRotationDG(pStation a);

// Fonctions principales
pStation creerStation(char* code, double cap);
pStation inserer(pStation a, char* code, double cap, double flux);
pStation rechercher(pStation a, char* code);
void liberer(pStation a);
void infixe(pStation a, FILE* fs);

// Gestion du réseau (Graphe)
void ajouterConnexion(pStation parent, pStation enfant, double fuite);

#endif
