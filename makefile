main: ex3.o linkedlist.o boundedbuffer.o unboundedbuffer.o
	gcc ex3.o linkedlist.o boundedbuffer.o unboundedbuffer.o -o ex3.out -lpthread

ex3.o: ex3.c
	gcc -c ex3.c -o ex3.o

linkedlist.o: linkedlist.c linkedlist.h
	gcc -c linkedlist.c -o linkedlist.o

boundedbuffer.o: boundedbuffer.c boundedbuffer.h
	gcc -c boundedbuffer.c -o boundedbuffer.o

unboundedbuffer.o: unboundedbuffer.c unboundedbuffer.h boundedbuffer.c boundedbuffer.h
	gcc -c unboundedbuffer.c -o unboundedbuffer.o

clean:
	rm -f ex3.out; rm -f *.o;