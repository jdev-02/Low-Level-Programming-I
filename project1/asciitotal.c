//File: asciitotal.c

//Name: Jonathan Goohs

//Description: This program takes two strings as input from the user. The first string can be any string, and the second must be an integer.
//The program will display the total of the ASCII values the user has inputted from the first string and then multiply that value by the second number.

//Syntax: This program has one function, main, which takes in two arguments, used by the following:  ./asciitotal string integer(multiplier)

//Reference: I used the type casting tutorial page to refresh myself on syntax for type casting in C. 
    //This can be found at https://tutorialspoint.com/cprogramming/c_type_casting.htm

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

#define BASE_10 10
#define NUM_ARGS 3
#define ARGV_S2A 1
#define ARGV_MULT 2

int main (int argc, char *argv[]) {
    //local vars
    long ascii_total = 0;
    long mult_val;
    long total_mult;
    errno = 0;
    
    //Verify the number of arguments from the user are 2
    if (argc != NUM_ARGS) {
        printf("You have specified the wrong number of arguments.\nExiting now...\n");
        exit(EXIT_FAILURE);
    }
    
    //For each char in argv[1] convert to ascii value and add to a var to show user and prepare for mulitplication
    for (int i=0;i<strlen(argv[ARGV_S2A]);i++) {
        //add the ith char's value to the ascii total
        ascii_total += (long)argv[ARGV_S2A][i];
    }
    printf("The ASCII total of the string = %ld\n",ascii_total);
    
    //if the argument entered is not a digit, isdigit will return non-zero*/
    for (int i=0;i<strlen(argv[ARGV_MULT]);i++) {
        if (isdigit(argv[ARGV_MULT][i]) == 0) {
            printf("You have not entered a proper value for multiplication of your ASCII value, it must be an integer within 1..100.\nExiting now...\n");
            exit(EXIT_FAILURE);
        }
    }  
    
    //Digits have been verified, now convert to long for multiplication with ascii_total
    mult_val = strtol(argv[ARGV_MULT], NULL, BASE_10);
    
    //strtol does not modify errno on success, so check if ernno changed and report possible errors and exit
    if (errno != 0) {
        printf("The provided second argument either has an unsupport value or the resulting value was out of range.\nExiting now...\n");
        exit(EXIT_FAILURE);
    }
    //verify the number is in bounds [1..100]
    if ((mult_val) < 1 || (mult_val) > 100) {
        printf("You have entered an invalid number for multiplication, must be between 1 and 100.\nExiting now...\n");
        exit(EXIT_FAILURE);
    }    
    
    //Now multiply the converted ascii value by the multiplier to get the total
    total_mult = ascii_total * mult_val;
    printf("The total * multiplier        = %ld\n",total_mult);
        
    //return success on function
    return 0;
}