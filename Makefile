all: FileReader

FileReader: FileReader.o
	gcc FileReader.o -o FileReader -lpthread

FileReader.o: FileReader.c
	gcc -c FileReader.c

clean: 
	rm -f FileReader.o FileReader
