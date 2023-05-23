all: main.o
	gcc -o main main.o

main.o: main.c
	gcc -c -o main.o main.c

# NUNCA PONER EN EL CLEAN LOS ARCHIVOS .C
# PARA BORRAR DIRECTORIOS SE DEBE COLOCAR r al -f, es decir colocar -rf
clean:
	rm -rf main main.o html latex
	
doc:
	doxygen
