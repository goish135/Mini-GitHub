all: OSclient OSserver

OSclient: OSclient.o
	gcc -o OSclient OSclient.o
OSserver: OSserver.o
	gcc -o OSserver OSserver.o
OSclient.o: OSclient.c
	gcc -c OSclient.c
OSserver.o: OSserver.c
	gcc -c OSserver.c

clean:
	rm -f OSserver.o OSclient.o
