all: proj4

proj4: proj4.o
	gcc proj4.o -o proj4 -lpthread

proj4.o: proj4.c
	gcc -c proj4.c

clean: 
	rm -f proj4.o proj4
