#include "avl.h"

// --- Fonctions utilitaires (Pas de string.h) ---

int comparerTexte(char* s1, char* s2) {
    int i = 0;
    while (s1[i] != '\0' && s2[i] != '\0') {
        if (s1[i] != s2[i]) {
            return (s1[i] - s2[i]);
        }
        i++;
    }
    return (s1[i] - s2[i]);
}

void copierTexte(char* dest, char* src) {
    int i = 0;
    while (src[i] != '\0') {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
}

// --- Fonctions AVL ---

int max(int a, int b) {
    return (a > b) ? a : b;
}

int hauteur(pStation a) {
    if (a == NULL) return 0;
    return a->h;
}

int equilibre(pStation a) {
    if (a == NULL) return 0;
    return hauteur(a->fg) - hauteur(a->fd);
}

// --- Création et Insertion ---

pStation creerStation(char* code, double cap) {
    pStation nouv = (pStation)malloc(sizeof(Station));
    if (nouv == NULL) {
        printf("Erreur d'allocation mémoire\n");
        exit(1);
    }
    copierTexte(nouv->id_str, code);
    nouv->capacite = cap;
    nouv->conso = 0;
    nouv->h = 1;
    nouv->fg = NULL;
    nouv->fd = NULL;
    return nouv;
}

pStation inserer(pStation a, char* code, double cap, double flux) {
    if (a == NULL) {
        pStation nouv = creerStation(code, cap);
        nouv->conso = flux;
        return nouv;
    }

    int cmp = comparerTexte(code, a->id_str);

    if (cmp < 0) {
        a->fg = inserer(a->fg, code, cap, flux);
    } 
    else if (cmp > 0) {
        a->fd = inserer(a->fd, code, cap, flux);
    } 
    else {
        // La station existe déjà : on CUMULE les données
        // C'est ici que la magie opère pour regrouper les tuyaux
        a->capacite += cap;
        a->conso += flux;
        return a;
    }

    // Rééquilibrage AVL
    a->h = 1 + max(hauteur(a->fg), hauteur(a->fd));
    int eq = equilibre(a);

    // Cas Gauche-Gauche
    if (eq > 1 && comparerTexte(code, a->fg->id_str) < 0) 
        return rotationDroite(a);

    // Cas Droite-Droite
    if (eq < -1 && comparerTexte(code, a->fd->id_str) > 0) 
        return rotationGauche(a);

    // Cas Gauche-Droite
    if (eq > 1 && comparerTexte(code, a->fg->id_str) > 0) 
        return doubleRotationGD(a);

    // Cas Droite-Gauche
    if (eq < -1 && comparerTexte(code, a->fd->id_str) < 0) 
        return doubleRotationDG(a);

    return a;
}

// --- Rotations ---

pStation rotationDroite(pStation y) {
    pStation x = y->fg;
    pStation T2 = x->fd;

    x->fd = y;
    y->fg = T2;

    y->h = max(hauteur(y->fg), hauteur(y->fd)) + 1;
    x->h = max(hauteur(x->fg), hauteur(x->fd)) + 1;

    return x;
}

pStation rotationGauche(pStation x) {
    pStation y = x->fd;
    pStation T2 = y->fg;

    y->fg = x;
    x->fd = T2;

    x->h = max(hauteur(x->fg), hauteur(x->fd)) + 1;
    y->h = max(hauteur(y->fg), hauteur(y->fd)) + 1;

    return y;
}

pStation doubleRotationGD(pStation a) {
    a->fg = rotationGauche(a->fg);
    return rotationDroite(a);
}

pStation doubleRotationDG(pStation a) {
    a->fd = rotationDroite(a->fd);
    return rotationGauche(a);
}

// --- Parcours et Libération ---

void infixe(pStation a, FILE* fs) {
    if (a != NULL) {
        infixe(a->fg, fs);
        // Format de sortie : ID;CAPACITE;CONSO
        // %.3f pour garder la précision du fichier .dat sans abuser
        fprintf(fs, "%s;%.6f;%.6f\n", a->id_str, a->capacite, a->conso);
        infixe(a->fd, fs);
    }
}

void liberer(pStation a) {
    if (a != NULL) {
        liberer(a->fg);
        liberer(a->fd);
        free(a);
    }
}
