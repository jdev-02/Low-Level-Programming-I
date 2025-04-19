#Name: Jonathan Goohs

#Description: This is the Makefile for project 1, configured to compile the asciitotal.c and tar file for submission.

all: asciitotal

asciitotal: asciitotal.c
	gcc -Wall asciitotal.c -o asciitotal

tar: Makefile asciitotal.c
	tar -cvf asciitotal.tar Makefile asciitotal.c

clean:
	rm -f asciitotal asciitotal.tar
