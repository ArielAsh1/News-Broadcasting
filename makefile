main: ex3.o linkedlist.o
	gcc ex3.o linkedlist.o -o ex3.out

ex3.o: ex3.c
	gcc -c ex3.c -o ex3.o

linkedlist.o: linkedlist.c linkedlist.h
	gcc -c linkedlist.c -o linkedlist.o

clean:
	rm -f ex3.out; rm -f *.o;