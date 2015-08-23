CC = gcc

INCLUDES = 

CFLAGS = -g -Wall $(INCLUDES)

LDFLAGS = -g

LDLIBS = 

ep1sh: ep1sh.o mallocSafe.o
#	$(CC) $(LDFLAGS) ep1sh.o mallocSafe.o $(LDLIBS)
#	(regra implicita do makefile)

ep1sh.o: ep1sh.c mallocSafe.h 
#	$(CC) -c $(CFLAGS) ep1sh.c



.PHONY: clean

clean:
	rm -f *.o 

.PHONY: all

all: clean main




