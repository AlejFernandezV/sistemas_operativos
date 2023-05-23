CC=gcc

all: main.o list.o sched.o plot.o split.o plot.o util.o
	$(CC) -o main main.o list.o sched.o split.o plot.o util.o

plot.o: plot.c plot.h
	$(CC) -c -o plot.o plot.c

list.o: list.c list.h
	$(CC) -c -o list.o list.c

sched.o: sched.c sched.h
	$(CC) -c -o sched.o sched.c

split.o: split.c split.h list.h util.h
	$(CC) -c -o split.o split.c

main.o: main.c split.h list.h
	$(CC) -c -o main.o main.c

util.o: util.c util.h
	$(CC) -c -o util.o util.c

clean:
	rm -rf *.o main main.exe gantt.gpi gantt.png docs
	rm -f test/*.png test/*.gpi
doc:
	doxygen