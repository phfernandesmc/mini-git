CC     = gcc
CFLAGS = -Wall -Wextra -std=c11 -g
OBJ    = main.o arquivo.o fila.o pilha.o lista.o commit.o diff.o

minigit: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o minigit
