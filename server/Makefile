CC?=gcc
RM?=rm -f

FILE_NAME=server

SOCKET=../socket/data.o ../socket/session.o
SERIALIZATION=../serialization/serialization.o

all: lib player_functions.o $(FILE_NAME).exe

lib: socket serialization

player_functions.o: player_functions.c player_functions.h
	$(CC) -c player_functions.c

$(FILE_NAME).exe: $(FILE_NAME).c $(FILE_NAME).h
	$(CC) -o $(FILE_NAME).exe $(FILE_NAME).c $(SOCKET) $(SERIALIZATION) player_functions.o -lpthread

socket:
	cd ../socket && $(MAKE)
serialization:
	cd ../serialization && $(MAKE)

clean:
	$(RM) *.o *.exe
	cd ../socket && $(MAKE) clean
	cd ../serialization && $(MAKE) clean