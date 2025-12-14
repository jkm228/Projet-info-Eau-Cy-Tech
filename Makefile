# Compilateur et options
CC = gcc
CFLAGS = -Wall -Wextra -std=c99
EXEC = c-wire

# Règle par défaut (lance la compilation de l'exécutable)
all: $(EXEC)

# Édition de liens : assemble les fichiers objets (.o) pour créer l'exécutable
$(EXEC): main.o avl.o file.o
	$(CC) $(CFLAGS) -o $(EXEC) main.o avl.o file.o

# --- Compilation séparée des modules ---

# main.o dépend de main.c mais aussi des headers qu'il inclut (avl.h, file.h)
main.o: main.c avl.h file.h
	$(CC) $(CFLAGS) -c main.c

# avl.o dépend de avl.c et avl.h
avl.o: avl.c avl.h
	$(CC) $(CFLAGS) -c avl.c

# file.o dépend de file.c, file.h et avl.h (car file.c utilise des types de avl.h)
file.o: file.c file.h avl.h
	$(CC) $(CFLAGS) -c file.c

# --- Nettoyage ---

# Supprime les fichiers temporaires (.o, exécutable, csv de sortie)
clean:
	rm -f *.o $(EXEC) *.csv

# Déclare que 'all' et 'clean' ne sont pas des vrais fichiers
.PHONY: all clean
