#Name: Jonathan Goohs

#Description: This is the Makefile for project 3, configured to compile the randadd.c and tar file for submission.

all: randadd

dots: randadd.c
	gcc -Wall -g randadd.c -o randadd

proj3.tar: Makefile randadd.c
	tar -cvf proj3.tar Makefile randadd.c

clean:
	rm -f randadd proj3.tar
