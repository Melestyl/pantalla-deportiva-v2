CC?=gcc
RM?=rm -f

FILE_NAME=server

SOCKET=../socket/data.o ../socket/session.o
SERIALIZATION=../serialization/serialization.o
FUNCTIONS=player_functions.o court_functions.o

all: lib $(FUNCTIONS) $(FILE_NAME).exe

lib: socket serialization

player_functions.o: player_functions.c player_functions.h
	$(CC) -c player_functions.c
court_functions.o: court_functions.c court_functions.h
	$(CC) -c court_functions.c

$(FILE_NAME).exe: $(FILE_NAME).c $(FILE_NAME).h
	$(CC) -o $(FILE_NAME).exe $(FILE_NAME).c $(SOCKET) $(SERIALIZATION) $(FUNCTIONS) -lpthread

socket:
	cd ../socket && $(MAKE)
serialization:
	cd ../serialization && $(MAKE)

clean:
	$(RM) *.o *.exe
	cd ../socket && $(MAKE) clean
	cd ../serialization && $(MAKE) clean