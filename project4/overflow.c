// ----------------------------------------------------------------------
// File: overflow.c
//
// Name: Jonathan Goohs
//
// Description: This file implements a program that will build a dynamically
//      allocated array with large random whole values, displaying those vals
//      with their running total, and denoting if there is overflow for an unsigned integer on a 32 bit system.
//      
// Syntax:
//      ./overflow number //number is the # of random values to be generated
//      for output, between 1 and 20, inclusive.
//
//Resources:
//1. www.scaler.com/topics/c/overflow-and-underflow-in-c/ for overflow checking reference
//2. stackoverflow.com/questions/18841167/invalid-read-in-valgrind for my read error troubleshooting
//3. calloc man page
//-----------------------

//Libraries
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

//Constants
#define VALID_NUM_ARGS 2
#define ARGV_PROGRAM   0
#define ARGV_NUMBER    1
#define MIN_NUMBER     1
#define MAX_NUMBER     20
#define MAX_VALUE      500500500
#define BASE_10        10
#define RED_TEXT "\033[31m"
#define RESET_COLOR "\033[0m"

//Return Vals
#define SUCCESS        0
#define BAD_POINTER    1
#define INVALID_NUMBER 2
#define MALLOC_ERROR   3
#define BAD_INPUT      4
#define BAD_BUILD      5
#define BAD_PRINT      6

// ------------------------ P R O T O T Y P E S -------------------------

int get_input(int argc, char *argv[], unsigned int *num);
int build_array(const int num, unsigned int value_list[]);
int print_values(unsigned int value_list[]);

// **************************  M  A  I  N  ******************************
//Function:
//  main
//inputs:
//  argc - # of cmd line args passed in
//  argv - the array of command line args passed in
//outputs:
//  result - returns 0 if program executes successfully, non-zero otherwise
//description:
//  1. obtains input through a call to get_input
//  2. dynamically allocate memory for an array using malloc
//  3. construct the list by calling build_list
//  4. print list values via print_values
//  5. frees dynamically alloating memory

int main (int argc, char *argv[]) {
    unsigned int num;
    int result               = SUCCESS;
    unsigned int *value_list = NULL;

    //random seed each time program runs
    srandom(time(NULL)*getpid());

    result = get_input(argc, argv, &num);
    //get num of args after error handling in function

    if (result == SUCCESS) {
        errno = SUCCESS;
        //include +1 for the null terminator, and multiply it by the value returned from argv[1]
        value_list = (unsigned int *)calloc(num+1, sizeof(unsigned int));
        //error handle for malloc
        if (value_list == NULL) {
            //memory allocation failure
                result = MALLOC_ERROR;
                perror("Unable to allocate memory for the array.");
                return result;
        }
    }

    // Build the dynamic array
    if (result == SUCCESS) {
        result = build_array(num, value_list);
        if (result != SUCCESS) {
            free(value_list);
            value_list = NULL;
            printf("Error: problem building array\n");
            return BAD_BUILD;
        }
    }

    // Print out dynamic array
    if (result == SUCCESS) {
        result = print_values(value_list);
        //free the dynamic array
        free(value_list);
        value_list = NULL;

        if (result != SUCCESS) {
            free(value_list);
            value_list = NULL;
            printf("Error printing the list\n");
            return BAD_PRINT;
        }
    }

    return result;
}

// ---------------------------------------------------------------------
// Function:
//     get_input
// Inputs:
//     argc
//         The value passed to main
//     argv
//         The value passed to main
// Outputs:
//     num
//         The number indicated by the string input by the user in
//         argv[ARGV_NUMBER].
//     function result:
//         An indicator of success or failure. A success is a zero,
//         while a failure is any other value.
// Description:
//     This function verifies that the user provided good input. If so,
//     it returns the integer equivalent of argv[ARGV_NUMBER].
// ---------------------------------------------------------------------
int get_input(int argc, char *argv[], unsigned int *num) {
    errno = 0; //for strtol call
    char *endnum = NULL; //for strtol call
    int result = SUCCESS;

    //handle bad input cases - bad arg count/wrong args/invalid number
    if (argc != VALID_NUM_ARGS) {
        printf("You may only enter two arguments: ./randadd <number>.\n");
        result = BAD_INPUT;
    } 
    //the user has inputed the correct arguments, now process the input they have entered
    long converted_input = strtol(argv[ARGV_NUMBER], &endnum, BASE_10);
    if (errno != 0 || *endnum != '\0' || argv[ARGV_NUMBER] == endnum ) { 
        //handle strtol errors
        printf("You have entered an invalid argument for the second argument, enter an integer.\n");
        result = BAD_INPUT;
    } 
    if (converted_input < MIN_NUMBER || converted_input > MAX_NUMBER) {
        printf("Please enter a number between 1-20 inclusive.\n");
        result = BAD_INPUT;
    }
    //all validating complete, now pass by reference to the caller and cast as an int
    *num = (int)converted_input;
    
    //all edge case for wrong input checked, so return result
    return result;
} // end get_input

// ---------------------------------------------------------------------
// Function:
//     build_list
// Inputs:
//     num
//         A number indicating how many objects need to be created
//         in the dynamic array
//     value_list[]
//          A block of memory of size specified by the num passed by
//          reference in the get_input function
// Outputs:
//     function result:
//         An indicator of success or failure. A success is a zero,
//         while a failure is any other value.
//     
// Description:
//     This function creates an array of random unsigned ints objects. The
//     number of objects to create is given as an input 'num'. In
//     addition, this function initializes the 'val' member of each
//     struct to a random integer in the range provided by MAX_VALUE.
// ---------------------------------------------------------------------
int build_array(const int num, unsigned int value_list[]){
    int result = SUCCESS;
    unsigned int i;
    
    if (value_list == NULL || num < 1)  {
        printf("Error: unexpected NULL pointer in build_list.\n");
        result = BAD_POINTER;
        return result;
    } else { //good pointer!
        //loop through for num times, generating random seeds and adding num to list
        for (i=0;i<num;i++){
            value_list[i] = rand() % MAX_VALUE;
        }
    }

    return result;
}


// ---------------------------------------------------------------------
// Function:
//     print_list
// Inputs:
//     start
//         A pointer to the start of a linked list.
// Outputs:
//     function result:
//         An indicator of success or failure. A success is a zero,
//         while a failure is any other value.
// Description:
//     This function walks through the linked list, printing out each
//     value in the list.
// ---------------------------------------------------------------------
int print_values(unsigned int value_list[]){
    int result = SUCCESS;
    unsigned int count = 1;
    unsigned int running_total = 0;
    unsigned int prev_total = 0;

    if (value_list == NULL) {
        result = BAD_POINTER;
        return result;
    }

    printf("\t Random    Running\n");
    printf("Count\t  Value\t     Total\n");
    printf("----- --------- ----------\n");
    //continue looping until you hit null terminator
    while (*value_list != '\0') {
        prev_total = running_total;
        //stores the previous count and updates current value with next item in list

        //check for unsigned int overflow
        if (__UINT32_MAX__ - prev_total < *value_list) {
            //print value that causes the overflow in red
            printf("%5d %s%9u%s %10u would cause overflow\n",count, RED_TEXT, *value_list, 
                RESET_COLOR, running_total);
        } else {
            running_total += *value_list;
            printf("%5d %9u %10u\n",count,*value_list,running_total);
            
        }
        //increment place in list and counter postfix
        value_list++;
        count++;
        }

    return result;
}