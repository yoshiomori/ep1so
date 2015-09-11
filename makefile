CC = gcc

INCLUDES = 

CFLAGS = -g -Wall $(INCLUDES)

LDFLAGS = -g -pthread

LDLIBS = 

.PHONY: all
all: ep1sh ep1

ep1sh: ep1sh.o mallocSafe.o leituraArq.o
#	$(CC) $(LDFLAGS) ep1sh.o mallocSafe.o $(LDLIBS)
#	(regra implicita do makefile)

ep1sh.o: ep1sh.c mallocSafe.h leituraArq.h
#	$(CC) -c $(CFLAGS) ep1sh.c

ep1: ep1.o mallocSafe.o

ep1.o: ep1.c mallocSafe.h

.PHONY: clean

clean:
	rm -f *.o 





