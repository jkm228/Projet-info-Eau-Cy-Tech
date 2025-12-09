#ifndef EN_TETE_H
#define EN_TETE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


Station* creerStation(int id, char* id_str, int capacite){
    Station* noeud;
    noeud = (Station*)malloc(sizeof(Station));
    if (noeud == NULL) {
        printf("Erreur d'allocation mémoire\n");
        exit(1);
    }

    noeud->id_numerique = id;
    noeud->id_str[49] = '\0'; // Sécurité pour la chaîne de caractères
    noeud->capacite = capacite;
    noeud->volume_traite = 0; // Initialisation logique à 0
    noeud->fils_gauche = NULL;
    noeud->fils_droit = NULL;
    noeud->hauteur = 1;
    return noeud;
}

Station* insererStation(Station* noeud, int id, char* id_str, int capacite, int volume_ajout){
    if (noeud == NULL) {
        Station* temp = creerStation(id, id_str, capacite);
        temp->volume_traite = volume_ajout; 
        return temp;
    }

    if (id < noeud->id_numerique)
        noeud->fils_gauche = insererStation(noeud->fils_gauche, id, id_str, capacite, volume_ajout);
    else if (id > noeud->id_numerique)
        noeud->fils_droit = insererStation(noeud->fils_droit, id, id_str, capacite, volume_ajout);
    else {
        // Égalité : Le nœud existe, on met à jour le volume (somme)
        noeud->volume_traite += volume_ajout;
        return noeud;
    }

    noeud->hauteur = 1 + max(hauteur(noeud->fils_gauche), hauteur(noeud->fils_droit));

    int equilibre = facteurEquilibre(noeud);

    if (equilibre >= 2) {
        // Si le fils droit est équilibré ou penche à droite -> Rotation Simple Gauche
        if (facteurEquilibre(noeud->fils_droit) >= 0) {
            return rotationGauche(noeud);
        } else {
            // Sinon -> Double Rotation Gauche
            return doubleRotationGauche(noeud);
        }
    }

    // Cas Gauche Lourd (Equilibre <= -2)
    if (equilibre <= -2) {
        if (facteurEquilibre(noeud->fils_gauche) <= 0) {
            return rotationDroite(noeud);
        } else {
            // Sinon -> Double Rotation Droite
            return doubleRotationDroite(noeud);
        }
    }

    return noeud; // Retourne le pointeur (inchangé si équilibré)
}

Station* rechercherStation(Station* racine, int id) {
    if (racine == NULL || racine->id_numerique == id)
        return racine;

    if (id < racine->id_numerique)
        return rechercherStation(racine->fg, id);
    
    return rechercherStation(racine->fd, id);
}

void parcoursInfixe(Station *racine, FILE* flux_sortie);

void libererAVL(Station* noeud) {
    if (noeud != NULL) {
        libererAVL(noeud->fils_gauche);
        libererAVL(noeud->fils_droit);
        free(noeud);
    }
}

// Prototypes utilitaires
int max(int a, int b);
int obtenirHauteur(Station *N);

#endif
