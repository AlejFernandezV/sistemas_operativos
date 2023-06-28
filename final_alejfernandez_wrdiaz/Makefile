all: main.o
	gcc -o hdisk main.o

main.o: main.c
	gcc -c -o main.o main.c

doc:
	doxygen

clean:
	rm -rf *.o hdisk.exe hdisk docs/