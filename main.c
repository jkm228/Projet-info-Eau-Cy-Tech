#include "wildwater.h"

// --- FONCTIONS UTILITAIRES ---
// Ces fonctions sont "static" car elles ne servent qu'à l'intérieur de ce fichier
// pour aider les fonctions principales.

// Fonction pour récupérer le maximum entre deux entiers
// Utile pour calculer la hauteur d'un nœud (1 + max(h_gauche, h_droit))
int max(int a, int b) {
    return (a > b) ? a : b;
}

// Récupère la hauteur d'un nœud de manière sécurisée
// Si le nœud est NULL (arbre vide), la hauteur est 0.
// Sinon, on renvoie la valeur stockée dans le champ 'hauteur'.
int hauteur(Station *N) {
    if (N == NULL) {
        return 0;
    }
    return N->hauteur;
}

// Calcule le facteur d'équilibre : Hauteur(Gauche) - Hauteur(Droit)
// Voir cours Info3_05_AVL.pdf
int facteurEquilibre(Station *N) {
    if (N == NULL) {
        return 0;
    }
    return hauteur(N->fils_gauche) - hauteur(N->fils_droit);
}

// --- FONCTIONS DE ROTATION (Voir Info3_05_AVL_rotations.pdf) ---

// Rotation Simple Droite (utilisée quand l'arbre est déséquilibré à Gauche-Gauche)
Station* rotationDroite(Station *y) {
    Station *x = y->fils_gauche;
    Station *T2 = x->fils_droit;

    // Rotation
    x->fils_droit = y;
    y->fils_gauche = T2;

    // Mise à jour des hauteurs (d'abord y car il est devenu enfant de x)
    y->hauteur = max(hauteur(y->fils_gauche), hauteur(y->fils_droit)) + 1;
    x->hauteur = max(hauteur(x->fils_gauche), hauteur(x->fils_droit)) + 1;

    // x devient la nouvelle racine de ce sous-arbre
    return x;
}

// Rotation Simple Gauche (utilisée quand l'arbre est déséquilibré à Droite-Droite)
Station* rotationGauche(Station *x) {
    Station *y = x->fils_droit;
    Station *T2 = y->fils_gauche;

    // Rotation
    y->fils_gauche = x;
    x->fils_droit = T2;

    // Mise à jour des hauteurs
    x->hauteur = max(hauteur(x->fils_gauche), hauteur(x->fils_droit)) + 1;
    y->hauteur = max(hauteur(y->fils_gauche), hauteur(y->fils_droit)) + 1;

    // y devient la nouvelle racine de ce sous-arbre
    return y;
}

// Double Rotation : Gauche puis Droite (Cas Gauche-Droite)
Station* doubleRotationGaucheDroite(Station *z) {
    z->fils_gauche = rotationGauche(z->fils_gauche);
    return rotationDroite(z);
}

// Double Rotation : Droite puis Gauche (Cas Droite-Gauche)
Station* doubleRotationDroiteGauche(Station *z) {
    z->fils_droit = rotationDroite(z->fils_droit);
    return rotationGauche(z);
}

// --- FONCTIONS PRINCIPALES DU PROJET ---

// Crée une nouvelle station (allocation mémoire)
Station* creerStation(int id, char* id_str, int capacite) {
    Station* noeud = (Station*)malloc(sizeof(Station));
    if (noeud == NULL) {
        printf("Erreur d'allocation mémoire pour la station %d\n", id);
        exit(1);
    }

    noeud->id_numerique = id;
    // Copie sécurisée de la chaîne (max 49 caractères + \0)
    strncpy(noeud->id_str, id_str, 49);
    noeud->id_str[49] = '\0'; 

    noeud->capacite = capacite;
    noeud->volume_traite = 0; // Initialisé à 0
    noeud->fils_gauche = NULL;
    noeud->fils_droit = NULL;
    noeud->hauteur = 1; // Un nouveau nœud est une feuille, hauteur 1

    return noeud;
}

// Insertion récursive avec équilibrage AVL automatique
Station* insererStation(Station* noeud, int id, char* id_str, int capacite, int volume_ajout) {
    // 1. Insertion normale d'ABR (Arbre Binaire de Recherche)
    if (noeud == NULL) {
        // Si on atteint une feuille vide, on crée la station ici
        Station* nouveau = creerStation(id, id_str, capacite);
        nouveau->volume_traite = volume_ajout; 
        return nouveau;
    }

    if (id < noeud->id_numerique) {
        noeud->fils_gauche = insererStation(noeud->fils_gauche, id, id_str, capacite, volume_ajout);
    } else if (id > noeud->id_numerique) {
        noeud->fils_droit = insererStation(noeud->fils_droit, id, id_str, capacite, volume_ajout);
    } else {
        // Cas d'égalité : La station existe déjà.
        // Selon le sujet : on cumule le volume traité (somme)
        noeud->volume_traite += volume_ajout;
        // On ne change rien à la structure de l'arbre, on retourne le nœud tel quel
        return noeud;
    }

    // 2. Mise à jour de la hauteur du nœud courant
    noeud->hauteur = 1 + max(hauteur(noeud->fils_gauche), hauteur(noeud->fils_droit));

    // 3. Vérification de l'équilibre
    int equilibre = facteurEquilibre(noeud);

    // 4. Cas de déséquilibre et Rotations

    // Cas Gauche Lourd (Left Heavy)
    if (equilibre > 1) {
        // Si le fils gauche penche aussi à gauche (ou est équilibré) -> Rotation Simple Droite
        if (facteurEquilibre(noeud->fils_gauche) >= 0) {
            return rotationDroite(noeud);
        } else {
            // Sinon (le fils gauche penche à droite) -> Double Rotation (Gauche-Droite)
            return doubleRotationGaucheDroite(noeud);
        }
    }

    // Cas Droit Lourd (Right Heavy)
    if (equilibre < -1) {
        // Si le fils droit penche aussi à droite (ou est équilibré) -> Rotation Simple Gauche
        if (facteurEquilibre(noeud->fils_droit) <= 0) {
            return rotationGauche(noeud);
        } else {
            // Sinon (le fils droit penche à gauche) -> Double Rotation (Droite-Gauche)
            return doubleRotationDroiteGauche(noeud);
        }
    }

    // Si le nœud est équilibré, on le retourne inchangé
    return noeud;
}

// Recherche récursive d'une station par son ID
Station* rechercherStation(Station* racine, int id) {
    if (racine == NULL || racine->id_numerique == id)
        return racine;
    
    if (id < racine->id_numerique)
        return rechercherStation(racine->fils_gauche, id);
    
    return rechercherStation(racine->fils_droit, id);
}

// Parcours Infixe (Gauche -> Racine -> Droite) pour écrire dans le fichier CSV
// Cela permet de sortir les stations triées par ID croissant
void parcoursInfixe(Station *racine, FILE* flux_sortie) {
    if (racine != NULL) {
        parcoursInfixe(racine->fils_gauche, flux_sortie);
        
        // Écriture formatée dans le fichier de sortie
        if (flux_sortie != NULL) {
            fprintf(flux_sortie, "%d;%d;%ld\n", 
                    racine->id_numerique, 
                    racine->capacite, 
                    (long)racine->volume_traite);
        }

        parcoursInfixe(racine->fils_droit, flux_sortie);
    }
}

// Libération de la mémoire (Parcours Post-fixe : enfants d'abord, puis racine)
void libererAVL(Station* noeud) {
    if (noeud != NULL) {
        libererAVL(noeud->fils_gauche);
        libererAVL(noeud->fils_droit);
        free(noeud);
    }
}
