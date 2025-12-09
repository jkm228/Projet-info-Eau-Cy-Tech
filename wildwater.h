#ifndef WILDWATER_H
#define WILDWATER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Station {
    int id_numerique;       // L'identifiant converti en nombre
    char id_str[50];        // L'identifiant complet (ex: "Facility complex...")
    int capacite;          // Capacité maximale (pour les usines)
    int volume_traite;     // Somme des volumes reçus
    struct Station *fils_gauche;
    struct Station *fils_droit;
    int hauteur;            // Hauteur du nœud pour l'équilibrage
} Station;

// Structure représentant un TUYAU 
typedef struct Connexion {
    struct Station* station_fils; // Vers qui va l'eau ?
    float pourcentage_fuite;      // Quelle quantité est perdue ici ? (colonne 5)
    struct Connexion* suivant;    // Liste chaînée des autres tuyaux partant du même parent
} Connexion;

Station* creerStation(int id, char* id_str, int capacite);
Station* insererStation(Station* noeud, int id, char* id_str, int capacite, int volume_ajout);
Station* rechercherStation(Station* racine, int id);
void parcoursInfixe(Station *racine, FILE* flux_sortie);
void libererAVL(Station* noeud);

#endif
