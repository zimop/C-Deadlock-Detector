CC=gcc
CFLAGS=-Wall
EXE_DETECT=detect
OBJ=detect.o list.o
$(EXE_DETECT): $(OBJ)
	$(CC) $(CFLAGS) -o $(EXE_DETECT) $(OBJ)

list.o: list.c list.h
	$(CC) $(CFLAGS) -c list.c

clean:
	rm -f $(OBJ) $(EXE_DETECT)