CC?=gcc
RM?=rm -f

SOCKET=../socket/data.o ../socket/session.o
SERIALIZATION=../serialization/serialization.o

all: lib server.exe

lib: socket serialization

server.exe: server.o
	$(CC) -o server.exe server.o $(SOCKET) $(SERIALIZATION) -lpthread
server.o: server.c server.h
	$(CC) -c server.c

socket:
	cd ../socket && $(MAKE)
serialization:
	cd ../serialization && $(MAKE)

clean:
	$(RM) *.o *.exe
	cd ../socket && $(MAKE) clean
	cd ../serialization && $(MAKE) clean