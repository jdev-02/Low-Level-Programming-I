# --------------------------------------------------------------------
# File: Makefile
#
# Name: Al Shaffer & Paul Clark
#
# Description: This is the Makefile for a simple YAHTZEE game. One of
#     the purposes of this project is to show how a multi-file
#     application gets compiled and linked.
#
#     Because there are so few .c files involved, it could've been done
#     much easier by just putting all the .c files in one gcc command,
#     but the hope is that the way it is done here can give the student
#     a feel for how a larger application would be compiled.
# --------------------------------------------------------------------

# This app will be built in two stages:
# 1) create object files from each .c file, then
# 2) link the object files into the application.

# The following line defines a macro to create all the required objects.
OBJECTS=main.o play.o score.o screen.o

# The following line defines a macro of all the required sources.
SOURCES=main.c play.c score.c screen.c

# The following line defines a macro of all the required headers.
HEADERS=play.h score.h screen.h

# The following sets all compile flags at once, allowing you to change
# them all in one place whenever needed.
CFLAGS=-Wall -c -g -Os

# Targets
all: yahtzee

yahtzee: $(OBJECTS)
	gcc $(OBJECTS) -o yahtzee

main.o: main.c play.h screen.h score.h
	gcc $(CFLAGS) main.c

play.o: play.c play.h score.h screen.h
	gcc $(CFLAGS) play.c

score.o: score.c score.h screen.h
	gcc $(CFLAGS) score.c

screen.o: screen.c screen.h
	gcc $(CFLAGS) screen.c

clean:
	rm -rf yahtzee $(OBJECTS) proj5.tar

proj5.tar: Makefile $(SOURCES) $(HEADERS)
	tar -cvf proj5.tar Makefile $(SOURCES) $(HEADERS)
