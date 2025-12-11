#ifndef WILDWATER_H
#define WILDWATER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// --- STRUCTURES (Celles de ton fichier) ---

typedef struct Station {
    int id_numerique;         // L'identifiant converti en nombre
    char id_str[50];          // L'identifiant complet (ex: "Facility complex...")
    int capacite;             // Capacité maximale (pour les usines)
    int volume_traite;        // Somme des volumes reçus
    struct Station *fils_gauche;
    struct Station *fils_droit;
    int hauteur;              // Hauteur du nœud pour l'équilibrage
} Station;

// Structure représentant un TUYAU (pour la partie Leaks)
typedef struct Connexion {
    struct Station* station_fils; // Vers qui va l'eau ?
    float pourcentage_fuite;      // Quelle quantité est perdue ici ? (colonne 5)
    struct Connexion* suivant;    // Liste chaînée des autres tuyaux
} Connexion;

// --- PROTOTYPES (Liste des fonctions disponibles) ---

// Fonctions principales (déjà présentes)
Station* creerStation(int id, char* id_str, int capacite);
Station* insererStation(Station* noeud, int id, char* id_str, int capacite, int volume_ajout);
Station* rechercherStation(Station* racine, int id);
void parcoursInfixe(Station *racine, FILE* flux_sortie);
void libererAVL(Station* noeud);

// --- AJOUTS INDISPENSABLES (Manquants dans ton fichier d'origine) ---
// Ces fonctions sont appelées dans avl.c, elles doivent être déclarées ici.

int max(int a, int b);
int hauteur(Station *N);
int facteurEquilibre(Station *N);

Station* rotationDroite(Station *y);
Station* rotationGauche(Station *x);
Station* doubleRotationDroite(Station *z);
Station* doubleRotationGauche(Station *z);

#endif
