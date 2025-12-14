CC = gcc
CFLAGS = -Wall -Wextra -std=c99
EXEC = c-wire

# Règle par défaut
all: $(EXEC)

# Édition de liens : on ajoute file.o
$(EXEC): main.o avl.o file.o
	$(CC) $(CFLAGS) -o $(EXEC) main.o avl.o file.o

# Compilation séparée des modules

main.o: main.c main.h avl.h file.h
	$(CC) $(CFLAGS) -c main.c

avl.o: avl.c avl.h
	$(CC) $(CFLAGS) -c avl.c

file.o: file.c file.h
	$(CC) $(CFLAGS) -c file.c

clean:
	rm -f *.o $(EXEC) stats.csv

.PHONY: all clean
