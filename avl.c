#include "wildwater.h"

// ... (Garde tes fonctions utilitaires max, hauteur, etc. et les rotations ici) ...

// Fonction insérer adaptée pour le tri ALPHABÉTIQUE
Station* insererStation(Station* noeud, int id, char* id_str, int capacite, int volume_ajout) {
    if (noeud == NULL) {
        Station* nouveau = creerStation(id, id_str, capacite);
        nouveau->volume_traite = volume_ajout; 
        return nouveau;
    }

    // COMPARAISON DE CHAINES (strcmp) au lieu d'entiers
    // strcmp(a, b) renvoie < 0 si a est avant b dans l'alphabet
    int cmp = strcmp(id_str, noeud->id_str);

    if (cmp < 0) {
        noeud->fils_gauche = insererStation(noeud->fils_gauche, id, id_str, capacite, volume_ajout);
    } else if (cmp > 0) {
        noeud->fils_droit = insererStation(noeud->fils_droit, id, id_str, capacite, volume_ajout);
    } else {
        // Égalité : C'est la même station, on ajoute le volume
        noeud->volume_traite += volume_ajout;
        return noeud;
    }

    // Mise à jour hauteur
    noeud->hauteur = 1 + max(hauteur(noeud->fils_gauche), hauteur(noeud->fils_droit));
    int equilibre = facteurEquilibre(noeud);

    // Équilibrage (reste identique sur le principe)
    if (equilibre > 1) {
        // Vérifie le fils gauche avec strcmp aussi
        if (strcmp(id_str, noeud->fils_gauche->id_str) < 0)
            return rotationDroite(noeud);
        else
            return doubleRotationGaucheDroite(noeud);
    }
    if (equilibre < -1) {
        if (strcmp(id_str, noeud->fils_droit->id_str) > 0)
            return rotationGauche(noeud);
        else
            return doubleRotationDroiteGauche(noeud);
    }

    return noeud;
}

// Recherche adaptée
Station* rechercherStation(Station* racine, int id, char* id_str) { // Ajout id_str
    if (racine == NULL || strcmp(racine->id_str, id_str) == 0)
        return racine;
    
    if (strcmp(id_str, racine->id_str) < 0)
        return rechercherStation(racine->fils_gauche, id, id_str);
    
    return rechercherStation(racine->fils_droit, id, id_str);
}

// ... (Garde parcoursInfixe et libererAVL) ...
