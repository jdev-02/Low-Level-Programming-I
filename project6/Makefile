# --------------------------------------------------------------------
# File: Makefile
#
# Name: Jonathan Goohs
#
# Description: This is the Makefile for a simple caesar cipher shift program.
# --------------------------------------------------------------------

# The following line defines a macro to create all the required objects.
OBJECTS=main.o shift.o

# The following line defines a macro of all the required sources.
SOURCES=main.c shift.c

# The following line defines a macro of all the required headers.
HEADERS=shift.h

# The following sets all compile flags at once, allowing you to change
# them all in one place whenever needed.
CFLAGS=-Wall -c -g

# Targets
all: shift

shift: $(OBJECTS)
	gcc $(OBJECTS) -o shift

main.o: main.c shift.h
	gcc $(CFLAGS) main.c

shift.o: shift.h shift.c
	gcc $(CFLAGS) shift.c

clean:
	rm -rf shift $(OBJECTS) proj6.tar

proj6.tar: Makefile $(SOURCES) $(HEADERS)
	tar -cvf proj6.tar Makefile $(SOURCES) $(HEADERS)
