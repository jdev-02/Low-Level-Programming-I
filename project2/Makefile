#Name: Jonathan Goohs

#Description: This is the Makefile for project 2, configured to compile the asciitotal.c and tar file for submission.

all: dots

dots: dots.c
	gcc -Wall dots.c -o dots

proj2.tar: Makefile dots.c
	tar -cvf proj2.tar Makefile dots.c

clean:
	rm -f dots proj2.tar
