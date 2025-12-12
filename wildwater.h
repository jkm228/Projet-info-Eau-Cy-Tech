#ifndef WILDWATER_H
#define WILDWATER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// --- STRUCTURES DE DONNÉES ---

// Structure pour l'AVL (Arbre Équilibré)
// Correspond aux nœuds décrits dans Info3_03_Arbres.pdf et Info3_05_AVL.pdf
typedef struct Station {
    int id_numerique;         // Identifiant unique
    char id_str[50];          // Identifiant textuel
    int capacite;             // Capacité (pour le max)
    long volume_traite;       // Volume cumulé (long pour éviter les dépassements)
    
    // Pointeurs vers les sous-arbres (Vocabulaire du cours)
    struct Station *fils_gauche;
    struct Station *fils_droit;
    
    // Hauteur du nœud pour le calcul du facteur d'équilibre
    int hauteur;              
} Station;

// Structure pour les Listes Chaînées (Graphe de connexion)
// Correspond à la structure "element" dans Info3_01_Listes_Chainees.pdf
typedef struct Connexion {
    struct Station* station_fils; // Donnée : Pointeur vers la station cible
    float pourcentage_fuite;      // Donnée : Information sur le lien
    struct Connexion* suivant;    // Pointeur vers l'élément suivant de la liste
} Connexion;


// --- PROTOTYPES DES FONCTIONS (Interface) ---

// 1. Fonctions Utilitaires AVL (Info3_05_AVL.pdf)
int max(int a, int b);
int hauteur(Station *N);
int facteurEquilibre(Station *N);

// 2. Fonctions de Rotation (Info3_05_AVL_rotations.pdf)
Station* rotationDroite(Station *y);
Station* rotationGauche(Station *x);
Station* doubleRotationGaucheDroite(Station *z);
Station* doubleRotationDroiteGauche(Station *z);

// 3. Fonctions Principales de Gestion de l'Arbre
Station* creerStation(int id, char* id_str, int capacite);
Station* insererStation(Station* noeud, int id, char* id_str, int capacite, int volume_ajout);
Station* rechercherStation(Station* racine, int id);

// 4. Fonctions d'Affichage et de Nettoyage
void parcoursInfixe(Station *racine, FILE* flux_sortie);
void libererAVL(Station* noeud);

#endif
