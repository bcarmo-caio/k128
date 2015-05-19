CC=gcc
BIN=k128
C_FLAGS=\
	-Wall\
	-std=c99\
	-pedantic\
	-O2

all:
	$(CC) -c *.c $(C_FLAGS)
	$(CC) *.o -o $(BIN)

clean:
	rm -rf *.o

del:
	rm -rf *.o $(BIN)

