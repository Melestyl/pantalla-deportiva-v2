CC?=gcc
RM?=rm -f

all: data.o session.o

data.o: data.c data.h
	$(CC) -c data.c

session.o: session.c session.h
	$(CC) -c session.c

clean:
	$(RM) *.o