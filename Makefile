CC=gcc
FLAGS=-lgdi32

tetris: tetris.c
	$(CC) tetris.c $(FLAGS) -o tetris

.PHONY: clean
clean:
	rm -rf tetris
	rm -rf *.o

