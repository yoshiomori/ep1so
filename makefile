CC = gcc

INCLUDES = 

CFLAGS = -g -Wall $(INCLUDES)

LDFLAGS = -g -pthread

LDLIBS = 

EXEC = ep1sh ep1

.PHONY: all
all: $(EXEC)

ep1sh: ep1sh.o mallocSafe.o leituraArq.o
#	$(CC) $(LDFLAGS) ep1sh.o mallocSafe.o $(LDLIBS)
#	(regra implicita do makefile)

ep1sh.o: ep1sh.c mallocSafe.h leituraArq.h
#	$(CC) -c $(CFLAGS) ep1sh.c

ep1: ep1.o mallocSafe.o filaDePrioridade.o

ep1.o: ep1.c mallocSafe.h filaDePrioridade.h

.PHONY: clean

clean:
	rm -f *.o $(EXEC)





