# ------------------------------------------------------------------------
#  File: Makefile
#
#  Name: Al Shaffer & Paul Clark
#
#  Description: This is the Makefile for project 7 in CS3040.
# ------------------------------------------------------------------------

OBJECTS=main.o display.o
CFLAGS=-Wall -c -g

all: clock

clock: $(OBJECTS)
	gcc $(OBJECTS) -lpthread -o clock

main.o: main.c display.h
	gcc $(CFLAGS) main.c

display.o: display.h display.c
	gcc $(CFLAGS) display.c

proj7.tar: main.c display.h display.c Makefile
	tar -cvf proj7.tar main.c display.h display.c Makefile

clean:
	rm -f clock $(OBJECTS) proj7.tar
