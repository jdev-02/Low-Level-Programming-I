// ------------------------------------------------------
// File: random.c
//
// Name: [Your Name]
//
// Description: 
//     Generates specified number of random values between 1-20 inclusive,
//     counts occurrences of each value, and displays count/frequency table.
//     Uses dynamic memory allocation for storing random values.
//
// Syntax:
//     ./random <number>   # number: positive integer up to 1,000,000
//
// Resources:
//     Class boilerplate code and lecture examples
// ------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>

// Constants for this specific program
#define SUCCESS            0
#define FAILURE            1
#define BAD_INPUT          2
#define MALLOC_ERROR       4
#define VALID_NUM_ARGS     2
#define ARGV_PROGRAM       0
#define ARGV_NUMBER        1
#define MIN_NUMBER         1
#define MAX_NUMBER         1000000
#define RANDOM_MIN         1
#define RANDOM_MAX         20
#define BASE_10            10
#define NUM_VALUES         20

// Function prototypes
int parse_arguments(int argc, char *argv[], int *count);
int *generate_random_values(int count);
void count_frequencies(int *values, int count, int *frequencies);
void print_results(int *frequencies, int total_count);
void init_random_seed(void);

// MAIN_BASIC: Main function for random value generation and counting
int main(int argc, char *argv[]) {
    int result = SUCCESS;
    int count = 0;
    int *random_values = NULL;
    int frequencies[NUM_VALUES] = {0}; // Initialize all to 0
    
    // Parse command line arguments
    result = parse_arguments(argc, argv, &count);
    if (result != SUCCESS) {
        return result;
    }
    
    // Initialize random seed for different results each run
    init_random_seed();
    
    // Generate random values in dynamically allocated memory
    random_values = generate_random_values(count);
    if (random_values == NULL) {
        return MALLOC_ERROR;
    }
    
    // Count frequency of each value (1-20)
    count_frequencies(random_values, count, frequencies);
    
    // Display results in required format
    print_results(frequencies, count);
    
    // Clean up allocated memory
    free(random_values);
    
    return SUCCESS;
}

// ARGS_SINGLE: Parse and validate single integer argument
int parse_arguments(int argc, char *argv[], int *count) {
    char *endptr = NULL;
    
    // Check argument count
    if (argc != VALID_NUM_ARGS) {
        fprintf(stderr, "Error: Invalid number of arguments.\n");
        fprintf(stderr, "Usage: %s <number>\n", argv[ARGV_PROGRAM]);
        fprintf(stderr, "  number: positive integer up to %d\n", MAX_NUMBER);
        return BAD_INPUT;
    }
    
    // Convert string to integer with error checking
    errno = 0;
    long converted = strtol(argv[ARGV_NUMBER], &endptr, BASE_10);
    
    // Check for conversion errors
    if (errno != 0) {
        fprintf(stderr, "Error: Invalid number format - %s\n", strerror(errno));
        return BAD_INPUT;
    }
    
    if (*endptr != '\0') {
        fprintf(stderr, "Error: Invalid number format - contains non-numeric characters.\n");
        return BAD_INPUT;
    }
    
    if (argv[ARGV_NUMBER] == endptr) {
        fprintf(stderr, "Error: No valid number provided.\n");
        return BAD_INPUT;
    }
    
    // Check range constraints
    if (converted < MIN_NUMBER || converted > MAX_NUMBER) {
        fprintf(stderr, "Error: Number must be between %d and %d inclusive.\n", 
                MIN_NUMBER, MAX_NUMBER);
        return BAD_INPUT;
    }
    
    *count = (int)converted;
    return SUCCESS;
}

// RANDOM_INIT: Initialize random seed for different results each run
void init_random_seed(void) {
    // Use time and process ID for better randomness
    srandom(time(NULL) * getpid());
}

// MALLOC_ARRAY: Generate array of random values using dynamic allocation
int *generate_random_values(int count) {
    if (count <= 0) {
        return NULL;
    }
    
    // Allocate memory for random values
    errno = 0;
    int *values = malloc(count * sizeof(int));
    
    if (values == NULL) {
        if (errno != 0) {
            fprintf(stderr, "Error: Memory allocation failed - %s\n", strerror(errno));
        } else {
            fprintf(stderr, "Error: Unexpected memory allocation failure\n");
        }
        return NULL;
    }
    
    // Fill array with random values between RANDOM_MIN and RANDOM_MAX
    for (int i = 0; i < count; i++) {
        values[i] = (random() % (RANDOM_MAX - RANDOM_MIN + 1)) + RANDOM_MIN;
    }
    
    return values;
}

// RANDOM_COUNT: Count occurrences of each value (1-20)
void count_frequencies(int *values, int count, int *frequencies) {
    if (values == NULL || frequencies == NULL || count <= 0) {
        return;
    }
    
    // Initialize frequency array to zero (should already be done but safety check)
    for (int i = 0; i < NUM_VALUES; i++) {
        frequencies[i] = 0;
    }
    
    // Count occurrences of each value
    for (int i = 0; i < count; i++) {
        // Ensure value is in valid range before using as array index
        if (values[i] >= RANDOM_MIN && values[i] <= RANDOM_MAX) {
            frequencies[values[i] - RANDOM_MIN]++; // Convert to 0-based index
        }
    }
}

// UTIL_PRINT: Display results in required format with count and frequency
void print_results(int *frequencies, int total_count) {
    if (frequencies == NULL || total_count <= 0) {
        return;
    }
    
    // Print header
    printf("Value Count Freq\n");
    printf("----- ----- -----\n");
    
    // Print results for each value that occurred
    for (int i = 0; i < NUM_VALUES; i++) {
        if (frequencies[i] > 0) {
            int value = i + RANDOM_MIN; // Convert back to actual value (1-20)
            double frequency = (double)frequencies[i] / total_count * 100.0;
            printf("%5d %5d %4.1f%%\n", value, frequencies[i], frequency);
        }
    }
}
