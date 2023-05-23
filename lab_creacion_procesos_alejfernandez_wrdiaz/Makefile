CC=gcc

all: main.o split.o 
	$(CC) -o main main.c split.c

main.o: main.c
	gcc -c -o main.o main.c

split.o: split.c 
	$(CC) -c -o split.o split.c

# NUNCA PONER EN EL CLEAN LOS ARCHIVOS .C
# PARA BORRAR DIRECTORIOS SE DEBE COLOCAR r al -f, es decir colocar -rf
clean:
	rm -rf main *.o docs
	
doc:
	doxygen 
