// ----------------------------------------------------------------------
// File: randadd.c
//
// Name: Al Shaffer & Paul Clark & Jonathan Goohs
//
// Description: This file implements a program that will build a list of
//     random values and add them together. The length of the list is
//     specified by the user. 
//     It will display the random values generated and their
//     total.
//
//     Internally, the list of values are generated first, and are
//     stored in a linked list. Each object of the list holds a
//     random value. The program then traverses through the list and
//     adds up the values in the list. The program then takes another
//     pass through the list to print out the values in the list as well
//     as their calculated total.
//
// Syntax:
//     ./randadd number   #number is an int between 1 and 42, inclusive
//
//Resources:
//www.geeksforgeeks.org/how-to-use-typedef-in-c to understand the struct definition. 
//
//Task 2: 
//I have nullified the current and start pointers in main after freeing the nodes.
//I changed the error checking for malloc for the start allocation from if errno == SUCCESS to if errno != SUCCESS.
//I improved the linked list freeing logic in main by:
//  -In the while loop, save the current node
//  -Move to the next node
//  -Free the previous (saved) node
//  -Continue until all nodes are freed
// ----------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

// constants
#define VALID_NUM_ARGS 2
#define ARGV_PROGRAM   0
#define ARGV_NUMBER    1
#define MIN_NUMBER     1
#define MAX_NUMBER     42
#define MAX_VALUE      50
#define BASE_10        10
#define VALID_EXECUTABLE "./randadd"
#define RED_TEXT "\033[31m"
#define RESET_COLOR "\033[0m"

// return codes
#define SUCCESS        0
#define BAD_POINTER    1
#define INVALID_NUMBER 2
#define MALLOC_ERROR   3
#define BAD_INPUT      4

// Define the structure type to be used in the linked list, where value_t is the variable to represent the struct value
typedef struct value value_t;
struct value {
    //struct value holds the members val, an integer, and a pointer to the structure called next
    int val;        // a random value
    value_t *next;  // link to next object in the list
};


// ----------------------------------------------------------------------
// ------------------------ P R O T O T Y P E S -------------------------
// ----------------------------------------------------------------------
int get_input(int argc, char *argv[], int *num);
int build_list(const int num, value_t *start);
int print_list(value_t *start);
int calc_total(value_t *start, long int *total);

// **********************************************************************
// **************************  M  A  I  N  ******************************
// **********************************************************************
int main(int argc, char *argv[])
{
    int num;
    int result       = SUCCESS;
    long int total   = -1;
    value_t *start   = NULL;
    value_t *current = NULL;

    // Verify the input from the user is valid
    result = get_input(argc, argv, &num);

    // Get the memory for the start of the list
    if (result == SUCCESS) {
        errno = SUCCESS;
        start = malloc(sizeof(value_t));
        if (start == NULL) {
            if (errno != SUCCESS) {
                result = MALLOC_ERROR;
                printf("malloc error when starting the list\n");
            } else {
                result = errno;
                perror("Unable to start the list");
            }
        }
    }

    // Build the linked list
    if (result == SUCCESS) {
        result = build_list(num, start);
        if (result != SUCCESS) {
            printf("Error: problem building list\n");
        }
    }

    // Print out linked list
    if (result == SUCCESS) {
        result = print_list(start);
        if (result != SUCCESS) {
            printf("Error printing the list\n");
        }
    }

    // Calculate and print the sum of the linked list
    if (result == SUCCESS) {
        result = calc_total(start, &total);
        if (result != SUCCESS) {
            printf("Error calculating total\n");
        } else {
            // Print the total
            printf("\nTotal = %ld\n", total);
        }
    }

    // Free up the linked list memory
    if (start != NULL) {
        current = start;
        while (current->next != NULL) {
            current = current->next;
            free(start);
            start = current; //the new head is now the next node
        }
    free(current);
    current = NULL;
    }
    start = NULL;
    
    return result;
} // end main

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
int get_input(int argc, char *argv[], int *num){
    errno = 0; //for strtol call
    char *endnum = NULL; //for strtol call
    int result = SUCCESS; //default success

    //handle bad input cases - bad arg count/wrong args/invalid number
    if (argc != VALID_NUM_ARGS) {
        printf("You may only enter two arguments: ./randadd <number>.\n");
        result = BAD_INPUT;
    } 
    if (strcmp(VALID_EXECUTABLE,argv[ARGV_PROGRAM]) != 0) {
        printf("You have entered the wrong executable file into the command line.\n");
        result = BAD_INPUT;
    } 
    //the user has inputed the correct arguments, now process the input they have entered
    long converted_input = strtol(argv[ARGV_NUMBER], &endnum, BASE_10);
    if (errno != 0 || *endnum != '\0' || argv[1] ==endnum ) {
        printf("You have entered an invalid argument for the second argument, enter an integer.\n");
        result = BAD_INPUT;
    } 
    if (converted_input < MIN_NUMBER || converted_input > MAX_NUMBER) {
        printf("Please enter a number between 1-42 inclusive.\n");
        result = BAD_INPUT;
    }
    //all validationg complete, now pass by reference to the caller and cast as an int
    *num = (int)converted_input;
    
    //all errors checked, so return result
    return result;
} // end get_input



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
int print_list(value_t *start)
{
    value_t *current = start; //start from the first node in the list

    int result = SUCCESS;

    if (start == NULL) {
        result = BAD_INPUT;
    } else {
        int count = 1;
        printf("\n"); // for formatting
        while (current != NULL) {
            //print current node in red as a random num right justified
            printf(RESET_COLOR "Value %2d = " RED_TEXT "%2d\n",count,current->val);
            current = current->next; //traversing the linked list
            count ++;
        }
        printf(RESET_COLOR); //reset color to black
    }

    return result;
} // end print_list



// ---------------------------------------------------------------------
// Function:
//     calc_total
// Inputs:
//     start
//         A pointer to the start of a linked list.
// Outputs:
//     total:
//         The total of all values in the input linked list.
//     function result:
//         An indicator of success or failure. A success is a zero,
//         while a failure is any other value.
// Description:
//     Given the linked list passed as input, this function will calculate
//     the total of each value within the list.
// ---------------------------------------------------------------------
int calc_total(value_t *start, long int *total)
{
    int result = SUCCESS;
    value_t *current = start; //start from first node in the linked list
    int count = 0; //initialize count with value in first node

    if (start == NULL) {
        result = BAD_INPUT;
    } else {
        while (current != NULL) {
            count += current->val; //accessing the val at that given node and adding to total
            current =  current->next; //traverse to the next node
        }
    
    *total = (long int)count;
    // return the total back to main
    }
    return result;
} // end calc_total



// ---------------------------------------------------------------------
// Function:
//     build_list
// Inputs:
//     num
//         A number indicating how many objects need to be created
//         in a linked list.
//     start
//         A pointer to memory that shall be used as the start of
//         the linked list.
// Outputs:
//     function result:
//         An indicator of success or failure. A success is a zero,
//         while a failure is any other value.
// Description:
//     This function creates a linked list of value_t objects. The
//     number of objects to create is given as an input 'num'. In
//     addition, this function initializes the 'val' member of each
//     struct to a random integer in the range provided by MAX_VALUE.
// ---------------------------------------------------------------------
int build_list(const int num, value_t *start)
{
    int result = SUCCESS;
    unsigned int i;
    value_t *last = NULL;
    value_t *new  = NULL;

    // verify passed pointer is valid
    if (start == NULL) {
        printf("Error: unexpected NULL pointer in build_list\n");
        result = BAD_POINTER;
    } else {    // we have a valid starting pointer
        // seed the random number generator
        srandom(time(NULL) * getpid());

        // initialize the initial struct
        start->val  = (random() % MAX_VALUE) + 1;
        start->next = NULL;

        // allocate additional structs and link them together
        for (i=1, last=start; i < num; ++i) {
            errno = SUCCESS;
            new = malloc(sizeof(value_t));
            if (new == NULL) {
                if (errno == SUCCESS) {
                    printf("Unexpected problem allocating memory.\n");
                    result = MALLOC_ERROR;
                } else {
                    perror("Memory allocation error");
                    result = errno;
                }
                break;
            }
            new->val   = (random() % MAX_VALUE) + 1;
            new->next  = NULL;
            last->next = new;
            last       = new;
        }
    }

    return result;
} // end build_list

// end randadd.c
