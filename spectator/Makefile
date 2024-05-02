CC?=gcc
RM?=rm -f

FILE_NAME=spectator

SOCKET=../socket/data.o ../socket/session.o
SERIALIZATION=../serialization/serialization.o

all: lib $(FILE_NAME).exe

lib: socket serialization

$(FILE_NAME).exe: $(FILE_NAME).c
	$(CC) -o $(FILE_NAME).exe $(FILE_NAME).c $(SOCKET) $(SERIALIZATION) -lpthread

socket:
	cd ../socket && $(MAKE)
serialization:
	cd ../serialization && $(MAKE)

clean:
	$(RM) *.o *.exe
	cd ../socket && $(MAKE) clean
	cd ../serialization && $(MAKE) clean