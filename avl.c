#include "avl.h"

// --- Fonctions utilitaires "Maison" (Pas de string.h) ---

int comparerTexte(char* s1, char* s2) {
    int i = 0;
    while (s1[i] != '\0' && s2[i] != '\0') {
        if (s1[i] != s2[i]) {
            return (s1[i] - s2[i]); // Retourne la différence ASCII
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
    if (a > b) return a;
    return b;
}

int hauteur(pStation a) {
    if (a == NULL) return 0;
    return a->h;
}

int equilibre(pStation a) {
    if (a == NULL) return 0;
    return hauteur(a->fg) - hauteur(a->fd);
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

// --- Gestion ---

pStation creerStation(int id, char* code, long cap) {
    pStation nouv = (pStation)malloc(sizeof(Station));
    if (nouv == NULL) exit(1);
    
    nouv->id = id;
    copierTexte(nouv->id_str, code);
    nouv->capacite = cap;
    nouv->conso = 0;
    nouv->h = 1;
    nouv->fg = NULL;
    nouv->fd = NULL;
    return nouv;
}

pStation inserer(pStation a, int id, char* code, long cap, long flux) {
    if (a == NULL) {
        pStation nouv = creerStation(id, code, cap);
        nouv->conso = flux;
        return nouv;
    }

    int cmp = comparerTexte(code, a->id_str);

    if (cmp < 0) {
        a->fg = inserer(a->fg, id, code, cap, flux);
    } 
    else if (cmp > 0) {
        a->fd = inserer(a->fd, id, code, cap, flux);
    } 
    else {
        // --- CORRECTION MAJEURE ICI ---
        // On additionne (+=) au lieu d'écraser (=) pour cumuler les lignes du fichier
        if (cap > 0) a->capacite += cap; 
        
        a->conso += flux;
        return a;
    }

    // Mise à jour hauteur et équilibrage
    a->h = 1 + max(hauteur(a->fg), hauteur(a->fd));
    int eq = equilibre(a);

    if (eq > 1 && comparerTexte(code, a->fg->id_str) < 0) return rotationDroite(a);
    if (eq < -1 && comparerTexte(code, a->fd->id_str) > 0) return rotationGauche(a);
    if (eq > 1 && comparerTexte(code, a->fg->id_str) > 0) return doubleRotationGD(a);
    if (eq < -1 && comparerTexte(code, a->fd->id_str) < 0) return doubleRotationDG(a);

    return a;
}

void infixe(pStation a, FILE* fs) {
    if (a != NULL) {
        infixe(a->fd, fs); // Parcours Inverse (Décroissant)
        if (fs != NULL) {
            fprintf(fs, "%s;%ld;%ld\n", a->id_str, a->capacite, a->conso);
        }
        infixe(a->fg, fs);
    }
}

void liberer(pStation a) {
    if (a != NULL) {
        liberer(a->fg);
        liberer(a->fd);
        free(a);
    }
}
