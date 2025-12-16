CC = gcc
CFLAGS = -Wall -Wextra -std=c99
EXEC = c-wire

all: $(EXEC)

$(EXEC): main.o avl.o file.o
	$(CC) $(CFLAGS) -o $(EXEC) main.o avl.o file.o

main.o: main.c avl.h file.h
	$(CC) $(CFLAGS) -c main.c

avl.o: avl.c avl.h
	$(CC) $(CFLAGS) -c avl.c

file.o: file.c file.h avl.h
	$(CC) $(CFLAGS) -c file.c

clean:
	rm -f *.o $(EXEC) *.csv *.png *.tmp

.PHONY: all clean
