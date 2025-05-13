#Name: Jonathan Goohs

#Description: This is the Makefile for project 4, configured to compile the overflow.c and tar file for submission.

all: overflow

overflow: overflow.c
	gcc -Wall -g -lm overflow.c -o overflow

proj4.tar: Makefile overflow.c
	tar -cvf proj4.tar Makefile overflow.c

clean:
	rm -f overflow proj4.tar
