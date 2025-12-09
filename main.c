#include "wildwater.h"

int main(int argc, char** argv) {
    Station* racine = NULL;

    // Exemple d'utilisation
    racine = insererStation(racine, 10, "Usine A", 5000, 100);
    racine = insererStation(racine, 5, "Source B", 0, 50);

    Station* s = rechercherStation(racine, 10);
    if (s != NULL) {
        printf("Station trouvée : %s\n", s->id_str);
    }

    libererAVL(racine);
    return 0;
}
