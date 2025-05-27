// ----------------------------------------------------------------------
// File: main.c
//
// Name: Jonathan Goohs
//
// Description: This is the main program for a simple caesar cipher shift program.
//
// Syntax: ./shift -e|-d key <input file> <output file> 
//
//Resources: strcmp man page
//size_t man page
//access man page for file existence checking and stackoverflow.com/questions/230062/whats-the-best-way-to-check-if-a-file-exists-in-c
// ----------------------------------------------------------------------

//Libraries
#include "shift.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stddef.h>
#include <unistd.h>

//Constants
#define VALID_NUM_ARGS 5
#define SUCCESS        0
#define BAD_ARGC       1
#define BAD_ARGV       2
#define BAD_KEY        3
#define BAD_INPUT_F    4
#define BAD_OUTPUT_F   5
#define BAD_SAME_FILE  6
#define MIN_NUMBER     0
#define MAX_NUMBER     255
#define ARGV_E_OR_D    1
#define ARGV_KEY       2
#define ARGV_INPUT_F   3
#define ARGV_OUTPUT_F  4
#define BUFSIZE        1024
#define BASE_10        10
#define ENCRYPT_CALL   0
#define DECRYPT_CALL   1
#define FWRITE_SIZE    1


// ------------------------ P R O T O T Y P E S -------------------------
int get_input(int argc, char *argv[], int *eord, unsigned int *key, FILE **input_fd, FILE **output_fd);

// *********************************  MAIN **********************************
//Function:
//  main
//inputs:
//  argc - # of cmd line args passed in (should be 4)
//  argv - the array of command line args passed in (should match syntax)
//outputs:
//  result - returns 0 if program executes successfully, error message and non-zero value otherwise
//description:
//  1. obtains input through a call to get_input
//  2. reads file within buffer
//  3. checks if e or d and then calls external function to apply to input function
//  4. write the encrypted or decrypted text to the output file
//  5. close the file and flush stdout

int main(int argc, char *argv[]) {
    int eord;
    unsigned int key;
    int result =      SUCCESS;
    FILE *input_fd =  NULL;
    FILE *output_fd = NULL;
    size_t bytes_read;
    char buffer[BUFSIZE];

    result = get_input(argc, argv, &eord, &key, &input_fd, &output_fd);
    if (result != 0){
        return EXIT_FAILURE;
    }
    //no errors with input, so now read input file content
    while ((bytes_read = fread(buffer, 1, BUFSIZE, input_fd)) > 0) {
        if (eord == ENCRYPT_CALL) {
            encrypt_file(buffer, bytes_read, key);
        }  
        if (eord == DECRYPT_CALL) {
            decrypt_file(buffer, bytes_read, key);
        }
        //if the input file has no data, still print an output file with no data
        fwrite(buffer, FWRITE_SIZE, bytes_read, output_fd);
    }
    //close files
    fclose(input_fd);
    fclose(output_fd);
    input_fd = NULL;
    output_fd = NULL;

    fflush(stdout);
    
    return EXIT_SUCCESS;

} // end main

// ---------------------------------------------------------------------
// Function:
//     get_input handles user input and passed values by reference for handling throughout the rest of the program.
// Inputs:
//     argc
//         The value passed to main
//     argv
//         The value passed to main
//     eord
//         the number corresponding to e or d in the flag passed to main
//     key
//          the key to shift by passed to main
//     input_fd
//      
// Outputs:
//      eord
//          the string -e or -d entered by the user, which will be passed back as 2 for encrypt or 3 for decrypt
//      key
//         The number of which to apply encryption or decryption by.
//      input_fd
//          if the file for input is valid, the input_fd is the file descriptor for the input file
//      output_fd
//          if the file for output is valid, the output_fd is the file descriptor for the output file
//     function result:
//         An indicator of success or failure. A success is a zero,
//         while a failure is any other value.
// Description:
//     This function verifies that the user provided good input. If so,
//     it passes the encryption/decryption value, key value, file descriptor for input and output all by reference.
// ---------------------------------------------------------------------
int get_input(int argc, char *argv[], int *eord, unsigned int *key, FILE **input_fd, FILE **output_fd){
    errno = 0;
    char *endnum = NULL; //for strtol call
    int result = SUCCESS;
    *input_fd = fopen(argv[ARGV_INPUT_F], "r");

    if (argc != VALID_NUM_ARGS) {
        fprintf(stderr, "Input must have 4 arguments, ./shift -e|-d key input output...exiting now.\n");
        return result = BAD_ARGC;
    }
    if (strcmp(argv[ARGV_E_OR_D],"-e") != 0 && strcmp(argv[ARGV_E_OR_D], "-d") != 0) {
        fprintf(stderr,"Your second argument must be the flags -e for encryption or -d for decryption...exiting now.\n");
        return result = BAD_ARGV;
    }
    if (strcmp(argv[ARGV_INPUT_F],argv[ARGV_OUTPUT_F]) == 0) {
        fprintf(stderr,"The input file and output file cannot be the same, enter unique file names...exiting now.\n");
        return result = BAD_SAME_FILE;
    }

    if (*input_fd == NULL) {
        perror("Error opening file...exiting program now due to");
        return result = BAD_INPUT_F;
    }
    
    if (access(argv[ARGV_OUTPUT_F], F_OK) == 0) {
        fprintf(stderr, "Output file specified already exists, cannot write over...exiting now.\n");
        fclose(*input_fd);
        return result = BAD_OUTPUT_F;
    } 
    errno = 0;      //reset errno since access sets errno and we want file to be written if it doesnt exist yet
    *output_fd = fopen(argv[ARGV_OUTPUT_F], "w");
    if (output_fd == NULL) {
        perror("Error opening output file...exiting now due to");
        fclose(*input_fd);
        return result = BAD_OUTPUT_F;
    }

    //since there is no existing file with that name, now open for writing and check if null
    *output_fd = fopen(argv[ARGV_OUTPUT_F], "w");

    if (*output_fd == NULL) {
        perror("Destination unable to be written to, specify a new location...exiting now because");
        fclose(*input_fd);       //need to close input file if theres an issue with writing to the output file
        return result = BAD_OUTPUT_F;
    }

    long converted_key = strtol(argv[ARGV_KEY], &endnum, BASE_10);
    if (errno != 0 || *endnum != '\0' || argv[ARGV_KEY] == endnum ) { 
        //handle strtol errors
        //maybe replace with fprintf(stderr, "unable to do x. stop signal: %s\n",strerror(errno));
        fprintf(stderr,"You have entered an invalid argument for the second argument, enter an integer.\n");
        return result = BAD_KEY;
    } 

    if (converted_key < MIN_NUMBER || converted_key > MAX_NUMBER) {
        fprintf(stderr,"Please enter a number between 0-255 inclusive.\n");
        return result = BAD_KEY;
    }

    //pass converted key by reference
    *key = (unsigned int)converted_key;

    if (strcmp(argv[ARGV_E_OR_D], "-e") == ENCRYPT_CALL) {
        //pass the encrypt option back to main for calling the shift.c function
        *eord = ENCRYPT_CALL;
    } else {        //decrypt call was chosen since the earlier case was vetted for non-e or d strings
        *eord = DECRYPT_CALL;
    }
    
    return result;

}



