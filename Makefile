C=gcc
CFLAGS=-pipe

all: assembler.o
	$(CC) $(CFLAGS) assembler.o -o assembler

myarr.o: assembler.c
	$(CC) $(CFLAGS) -c assembler.c

clean: 
	rm assembler *.o *.*~ *~ 

