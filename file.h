#ifndef FILE_H
#define FILE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAILLE_LIGNE 1024

FILE* ouvrir_fichier(char* chemin, char* mode);
void fermer_fichier(FILE* f);

// Modification : id_station devient un buffer de caractères (char*)
int lire_ligne_csv(FILE* f, char* id_station, long* capacite);

#endif
