//File: dots.c

//Name: Jonathan Goohs

//Description: The program clears the current terminal screen, and then produces a number of dots on to the terminal screen,
//specified by the user via the command line.

//Syntax: The user is expected to run the program by the following: ./dots #_of_dots_you_want_to_see

//References:
//man pages for srandom(), strtol(), time(), random(), tiocswinsz(), string.h docs library
//gnu.org/software/gnu-c-manual/gnu-c-manual.html#pointers
//gist.github.com/fnky/458719343aabd01cfb17a3a4f7296797 for exploring escape code syntax

//libraries
#include <stdlib.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <string.h>

//defines
#define ARGV1 1
#define ARGV0 0 
#define CLEAR_SCREEN "\x1b[2J"
#define MOVE_ANDPRINTDOT_CURSOR "\x1b[%i;%iH%c"
#define MOVE_CURSOR "\x1b[%i;%iH"
#define SLEEP_TIME 1
#define MIN_COL 1
#define FINAL_ROW 1
#define REQ_ARGS 2
#define DECIMAL 10

//prototypes
int display_dots(const long int number);
//returns an int indicating success/failure, takes in a long int of specified number

int main(int argc, char *argv[]){
    errno = 0;
    if (argc != REQ_ARGS){
        printf("The program use guide specifies ./dots #_of_dots_you_want_to_see. Enter only two arguments.\n");
        exit(EXIT_FAILURE);
    } if (strcmp(argv[ARGV0],"./dots") != 0) {
        printf("Enter proper first argument of ./dots.\n");
        exit(EXIT_FAILURE);
    }
    
    char *end_ptr = NULL;
    const long int user_dots = strtol(argv[ARGV1],&end_ptr,DECIMAL);
    
    if (*end_ptr != '\0') {
        //if the last char in the string entered is not the null char, strol() stores address of the first invalid char in *endptr
        printf("You have not entered a valid integer for this program...Exiting now.\n");
        exit(EXIT_FAILURE);
    }
    if (errno != 0) {
        printf("The second argument is specified number of dots, i.e. 50. Enter an integer between 1-100.\n");
        exit(EXIT_FAILURE);
    }
    if ((user_dots <1) || (user_dots >100)) {
        printf("Number is not in proper range, enter a number between 1-100.\n");
        exit(EXIT_FAILURE);
    }
    //all errors handled dealing with user, now do intended functionality
    //srandom taked in an unsigned int and returns nothing
    srandom(time(NULL) * getpid());
    
    //clear current screen of user
    printf(CLEAR_SCREEN);
    if (display_dots(user_dots) == 1) {
        printf("Error printing dots, exiting now...\n.");
        exit(EXIT_FAILURE);
    }
    return 0;
}


int display_dots(const long int number) {
    //this function takes in the user specified number, then uses that to find the largest row column for that number
    //and then prints to screen the specified number of dots on to random coordinates in the terminal space
    //returns a 0 or 1 based on success of the program back to main.
    int result = 0;
    int max_rows;
    int max_cols;
    int rand_row;
    int rand_col;
    struct winsize win;

    errno = 0;
    result = ioctl(STDIN_FILENO, TIOCGWINSZ, &win);
    if ((result < 0) || (errno !=0)) {
        //on success of ioctl 0 is returned. on error -1 is returned
        printf("There was an error when interacting with the terminal screen.\n");
        return 1;
    }
    //gets current users terminal rows
    max_rows = win.ws_row-FINAL_ROW; 
    //gets current users terminal columns
    max_cols = win.ws_col;
    //want to produce dots on range of 1,1 to maxrows-1,maxcol
    //determine the random coords for a dot
    
    for (int i=0; i<number;i++){
        rand_row = rand() % (max_rows);
        rand_col = rand() % (max_cols);
        //move cursor to x and y coords and print the dot at that coord
        printf(MOVE_ANDPRINTDOT_CURSOR, rand_row,rand_col,'*');
        fflush(stdout);
        sleep(SLEEP_TIME);
    }
    //done printing, move cursor to bottom left of terminal and return 0
    printf(MOVE_CURSOR,max_rows+FINAL_ROW,MIN_COL);
    return 0;
    
}



