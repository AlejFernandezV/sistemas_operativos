all: productor_consumidor.o filosofos.o 
	gcc -o productor_consumidor productor_consumidor.o -pthread
	gcc -o filosofos filosofos.o -pthread

productor_consumidor.o: productor_consumidor.c
	gcc -c -o productor_consumidor.o productor_consumidor.c

filosofos.o: filosofos.c 
	gcc -c -o filosofos.o filosofos.c

# NUNCA PONER EN EL CLEAN LOS ARCHIVOS .C
# PARA BORRAR DIRECTORIOS SE DEBE COLOCAR r al -f, es decir colocar -rf
clean:
	rm -f  *.o productor_consumidor filosofos
	
doc:
	doxygen 