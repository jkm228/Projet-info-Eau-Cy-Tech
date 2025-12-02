#ifndef EN_TETE_H
#define EN_TETE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Structure représentant un nœud de l'arbre AVL (une Station ou Usine)
typedef struct Station {
    int id_numerique;       // L'identifiant converti en nombre
    char id_str[50];        // L'identifiant complet (ex: "Facility complex...")
    long capacite;          // Capacité maximale (pour les usines)
    long volume_traite;     // Somme des volumes reçus
    struct Station *fils_gauche;
    struct Station *fils_droit;
    int hauteur;            // Hauteur du nœud pour l'équilibrage
} Station;

// Prototypes des fonctions AVL
Station* creerStation(int id, char* id_str, long capacite);
Station* insererStation(Station* noeud, int id, char* id_str, long capacite, long volume_ajout);
void parcoursInfixe(Station *racine, FILE* flux_sortie);
void libererAVL(Station* noeud);

// Prototypes utilitaires
int max(int a, int b);
int obtenirHauteur(Station *N);

#endif
