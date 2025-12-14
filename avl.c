#include "wildwater.h"

// --- UTILITAIRES ---
int max(int a, int b) {
    return (a > b) ? a : b;
}

int hauteur(Station *N) {
    if (N == NULL) return 0;
    return N->hauteur;
}

int facteurEquilibre(Station *N) {
    if (N == NULL) return 0;
    return hauteur(N->fils_gauche) - hauteur(N->fils_droit);
}

// --- ROTATIONS ---
Station* rotationDroite(Station *y) {
    Station *x = y->fils_gauche;
    Station *T2 = x->fils_droit;
    x->fils_droit = y;
    y->fils_gauche = T2;
    y->hauteur = max(hauteur(y->fils_gauche), hauteur(y->fils_droit)) + 1;
    x->hauteur = max(hauteur(x->fils_gauche), hauteur(x->fils_droit)) + 1;
    return x;
}

Station* rotationGauche(Station *x) {
    Station *y = x->fils_droit;
    Station *T2 = y->fils_gauche;
    y->fils_gauche = x;
    x->fils_droit = T2;
    x->hauteur = max(hauteur(x->fils_gauche), hauteur(x->fils_droit)) + 1;
    y->hauteur = max(hauteur(y->fils_gauche), hauteur(y->fils_droit)) + 1;
    return y;
}

Station* doubleRotationGaucheDroite(Station *z) {
    z->fils_gauche = rotationGauche(z->fils_gauche);
    return rotationDroite(z);
}

Station* doubleRotationDroiteGauche(Station *z) {
    z->fils_droit = rotationDroite(z->fils_droit);
    return rotationGauche(z);
}

// --- GESTION ARBRE ---

Station* creerStation(int id, char* id_str, int capacite) {
    Station* noeud = (Station*)malloc(sizeof(Station));
    if (noeud == NULL) exit(1);
    
    noeud->id_numerique = id;
    strcpy(noeud->id_str, id_str); // Copie du texte
    noeud->capacite = capacite;
    noeud->volume_traite = 0;
    noeud->fils_gauche = NULL;
    noeud->fils_droit = NULL;
    noeud->hauteur = 1;
    return noeud;
}

Station* insererStation(Station* noeud, int id, char* id_str, int capacite, int volume_ajout) {
    if (noeud == NULL) {
        Station* temp = creerStation(id, id_str, capacite);
        temp->volume_traite = volume_ajout; 
        return temp;
    }

    // IMPORTANT : Comparaison de TEXTE (strcmp)
    int cmp = strcmp(id_str, noeud->id_str);

    if (cmp < 0)
        noeud->fils_gauche = insererStation(noeud->fils_gauche, id, id_str, capacite, volume_ajout);
    else if (cmp > 0)
        noeud->fils_droit = insererStation(noeud->fils_droit, id, id_str, capacite, volume_ajout);
    else {
        // La station existe déjà, on ajoute le volume
        noeud->volume_traite += volume_ajout;
        return noeud;
    }

    // Equilibrage
    noeud->hauteur = 1 + max(hauteur(noeud->fils_gauche), hauteur(noeud->fils_droit));
    int equilibre = facteurEquilibre(noeud);

    if (equilibre > 1) {
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

// Recherche (optionnelle pour l'instant)
Station* rechercherStation(Station* racine, int id, char* id_str) {
    if (racine == NULL || strcmp(racine->id_str, id_str) == 0)
        return racine;
    if (strcmp(id_str, racine->id_str) < 0)
        return rechercherStation(racine->fils_gauche, id, id_str);
    return rechercherStation(racine->fils_droit, id, id_str);
}

// --- AFFICHAGE (Celle qui manquait !) ---
void parcoursInfixe(Station *racine, FILE* flux_sortie) {
    if (racine != NULL) {
        parcoursInfixe(racine->fils_gauche, flux_sortie);
        
        // C'est ICI qu'on écrit dans le fichier stats.csv
        // Format : ID;CAPACITÉ;VOLUME
        if (flux_sortie != NULL) {
            fprintf(flux_sortie, "%s;%ld;%ld\n", 
                    racine->id_str, 
                    racine->volume_traite, // On utilise volume_traite comme valeur accumulée
                    racine->volume_traite);
        }

        parcoursInfixe(racine->fils_droit, flux_sortie);
    }
}

void libererAVL(Station* noeud) {
    if (noeud != NULL) {
        libererAVL(noeud->fils_gauche);
        libererAVL(noeud->fils_droit);
        free(noeud);
    }
}
