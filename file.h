#ifndef FILE_H
#define FILE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// On définit une constante pour la taille max d'une ligne
#define TAILLE_LIGNE 1024

// Prototypes des fonctions implémentées dans file.c

// Ouvre un fichier et gère les erreurs (arrête le programme si échec)
FILE* ouvrir_fichier(char* chemin, char* mode);

// Ferme le fichier
void fermer_fichier(FILE* f);

// Lit une ligne du CSV et extrait les informations utiles
// Retourne 1 si une ligne a été lue, 0 si fin de fichier
// Utilise des pointeurs pour modifier les variables du main (Passage par adresse)
int lire_ligne_csv(FILE* f, int* id_station, long* capacite, long* debit);

#endif
