#include "avl.h"

// Utile pour les hauteurs
int max(int a, int b) {
    if (a > b) return a;
    return b;
}

// Récupère la hauteur d'un noeud (gère le cas NULL)
int hauteur(pStation a) {
    if (a == NULL) return 0;
    return a->h;
}

// Calcul du facteur d'équilibre : h(fg) - h(fd)
int equilibre(pStation a) {
    if (a == NULL) return 0;
    return hauteur(a->fg) - hauteur(a->fd);
}

// --- Rotations Simples ---

pStation rotationDroite(pStation y) {
    pStation x = y->fg;
    pStation T2 = x->fd;

    // Rotation
    x->fd = y;
    y->fg = T2;

    // Mise à jour des hauteurs
    y->h = max(hauteur(y->fg), hauteur(y->fd)) + 1;
    x->h = max(hauteur(x->fg), hauteur(x->fd)) + 1;

    return x; // Nouvelle racine
}

pStation rotationGauche(pStation x) {
    pStation y = x->fd;
    pStation T2 = y->fg;

    // Rotation
    y->fg = x;
    x->fd = T2;

    // Mise à jour des hauteurs
    x->h = max(hauteur(x->fg), hauteur(x->fd)) + 1;
    y->h = max(hauteur(y->fg), hauteur(y->fd)) + 1;

    return y;
}

// --- Rotations Doubles ---

pStation doubleRotationGD(pStation a) {
    a->fg = rotationGauche(a->fg);
    return rotationDroite(a);
}

pStation doubleRotationDG(pStation a) {
    a->fd = rotationDroite(a->fd);
    return rotationGauche(a);
}

// --- Gestion de l'arbre ---

pStation creerStation(int id, char* code, long cap) {
    pStation nouv = (pStation)malloc(sizeof(Station));
    if (nouv == NULL) {
        exit(1); // Erreur alloc
    }
    nouv->id = id;
    strcpy(nouv->id_str, code);
    nouv->capacite = cap;
    nouv->conso = 0;
    nouv->h = 1;
    nouv->fg = NULL;
    nouv->fd = NULL;
    return nouv;
}

pStation inserer(pStation a, int id, char* code, long cap, long flux) {
    // 1. Insertion ABR classique
    if (a == NULL) {
        pStation nouv = creerStation(id, code, cap);
        nouv->conso = flux;
        return nouv;
    }

    int cmp = strcmp(code, a->id_str);

    if (cmp < 0) {
        a->fg = inserer(a->fg, id, code, cap, flux);
    } 
    else if (cmp > 0) {
        a->fd = inserer(a->fd, id, code, cap, flux);
    } 
    else {
        // Le noeud existe déjà : on met à jour les données
        if (cap > 0) a->capacite = cap;
        a->conso += flux;
        return a;
    }

    // 2. Mise à jour hauteur
    a->h = 1 + max(hauteur(a->fg), hauteur(a->fd));

    // 3. Équilibrage (AVL)
    int eq = equilibre(a);

    // Cas Gauche-Gauche
    if (eq > 1 && strcmp(code, a->fg->id_str) < 0) {
        return rotationDroite(a);
    }
    // Cas Droite-Droite
    if (eq < -1 && strcmp(code, a->fd->id_str) > 0) {
        return rotationGauche(a);
    }
    // Cas Gauche-Droite
    if (eq > 1 && strcmp(code, a->fg->id_str) > 0) {
        return doubleRotationGD(a);
    }
    // Cas Droite-Gauche
    if (eq < -1 && strcmp(code, a->fd->id_str) < 0) {
        return doubleRotationDG(a);
    }

    return a;
}

// Parcours infixe inverse (Droit -> Racine -> Gauche) pour tri décroissant
void infixe(pStation a, FILE* fs) {
    if (a != NULL) {
        infixe(a->fd, fs);
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
