all: libmem.so main

libmem.so: libmem.o
	gcc -shared -o libmem.so libmem.o
libmem.o: libmem.c mem.h
	gcc -c -fpic libmem.c -Wall -Werror
main: main.c mem.h
	gcc -o main main.c -lmem -L. -Wall -Werror
clean:
	rm -f main libmem.o libmem.so
