Station* creerStation(int id, char* id_str, int capacite);
Station* insererStation(Station* noeud, int id, char* id_str, int capacite, int volume_ajout);
Station* rechercherStation(Station* racine, int id);
void parcoursInfixe(Station *racine, FILE* flux_sortie);
void libererAVL(Station* noeud);

#endif
