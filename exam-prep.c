// ------------------------------------------------------
// Enhanced C Boilerplate Snippet Library for Exam
// Copy-paste the relevant sections below into your <program>.c
// Based on class style guide and examples
//
// SEARCH TAGS: Use Ctrl+F to find sections quickly:
// - HEADER: File header template
// - INCLUDES: Standard includes and macros
// - ARGS: Argument parsing
// - MALLOC: Dynamic memory allocation
// - LIST: Linked list operations
// - RANDOM: Random number generation
// - FILE: File I/O operations
// - PROCESS: Process management/fork/exec
// - THREAD: Threading and mutex
// - SIGNAL: Signal handling
// - MAIN: Main function templates
// - UTIL: Utility functions
// ------------------------------------------------------

// 1) --- HEADER: File Header Template ---
// ------------------------------------------------------
// File: <program>.c
//
// Name: [Your Name]
//
// Description: 
//     [Program description - what it does, how it works internally]
//
// Syntax:
//     ./<program> <args>   #[arg description]
//
// Resources:
//     [Any references used]
// ------------------------------------------------------


// 2) --- INCLUDES: Standard Includes & Macros ---
// ------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <pthread.h>
#include <sys/resource.h>
#include <signal.h>
#include <stdbool.h>

// CONSTANTS: Common error codes and limits
#define SUCCESS            0
#define FAILURE            1
#define BAD_INPUT          2
#define BAD_POINTER        3
#define MALLOC_ERROR       4
#define VALID_NUM_ARGS     2
#define ARGV_PROGRAM       0
#define ARGV_NUMBER        1
#define MIN_NUMBER         1
#define MAX_NUMBER         100
#define BASE_10            10
#define THREAD_COUNT       4
#define BUFFER_SIZE        1024

// Unused parameter macro
#define UNUSED(x)          ((void)(x))


// 3) --- ARGS: Argument Parsing Templates ---
// ------------------------------------------------------

// ARGS_SINGLE: Parse single integer argument
int parse_single_int(int argc, char *argv[], int *num) {
    char *endptr = NULL;
    
    if (argc != VALID_NUM_ARGS) {
        printf("Usage: %s <number>\n", argv[ARGV_PROGRAM]);
        return BAD_INPUT;
    }
    
    errno = 0;
    long converted = strtol(argv[ARGV_NUMBER], &endptr, BASE_10);
    
    if (errno != 0 || *endptr != '\0' || argv[ARGV_NUMBER] == endptr) {
        printf("Invalid number format.\n");
        return BAD_INPUT;
    }
    
    if (converted < MIN_NUMBER || converted > MAX_NUMBER) {
        printf("Number must be between %d and %d inclusive.\n", MIN_NUMBER, MAX_NUMBER);
        return BAD_INPUT;
    }
    
    *num = (int)converted;
    return SUCCESS;
}

// ARGS_MULTI: Parse multiple arguments (int + string example)
int parse_multiple_args(int argc, char *argv[], int *arg1, char **arg2) {
    if (argc != 3) {
        printf("Usage: %s <number> <string>\n", argv[0]);
        return BAD_INPUT;
    }
    
    // Parse first argument as integer
    char *endptr = NULL;
    errno = 0;
    long converted = strtol(argv[1], &endptr, BASE_10);
    
    if (errno != 0 || *endptr != '\0' || converted < 1) {
        printf("First argument must be a positive integer.\n");
        return BAD_INPUT;
    }
    
    *arg1 = (int)converted;
    *arg2 = argv[2];  // String argument
    
    return SUCCESS;
}


// 4) --- MALLOC: Dynamic Memory Management Templates ---
// ------------------------------------------------------

// MALLOC_ARRAY: Basic array allocation
int *allocate_int_array(int size) {
    if (size <= 0) return NULL;
    
    errno = 0;
    int *array = malloc(size * sizeof(int));
    
    if (array == NULL) {
        if (errno != 0) {
            perror("malloc failed");
        } else {
            printf("Unexpected malloc failure\n");
        }
        return NULL;
    }
    
    return array;
}

// REALLOC: Safe array reallocation
int *reallocate_int_array(int *array, int old_size, int new_size) {
    if (new_size <= 0) return NULL;
    
    errno = 0;
    int *new_array = realloc(array, new_size * sizeof(int));
    
    if (new_array == NULL) {
        perror("realloc failed");
        return array;  // Return original array if realloc fails
    }
    
    // Initialize new elements if expanding
    if (new_size > old_size) {
        for (int i = old_size; i < new_size; i++) {
            new_array[i] = 0;
        }
    }
    
    return new_array;
}


// 5) --- LIST: Linked List Templates ---
// ------------------------------------------------------

// LIST_STRUCT: Generic linked list node structure
typedef struct node node_t;
struct node {
    int data;
    node_t *next;
};

// LIST_CREATE: Create new node
node_t *create_node(int value) {
    errno = 0;
    node_t *new_node = malloc(sizeof(node_t));
    
    if (new_node == NULL) {
        if (errno != 0) {
            perror("malloc failed for new node");
        } else {
            printf("Unexpected malloc failure\n");
        }
        return NULL;
    }
    
    new_node->data = value;
    new_node->next = NULL;
    return new_node;
}

// LIST_BUILD: Build linked list from array
int build_linked_list(int *values, int count, node_t **head) {
    if (values == NULL || count <= 0 || head == NULL) {
        return BAD_POINTER;
    }
    
    *head = create_node(values[0]);
    if (*head == NULL) return MALLOC_ERROR;
    
    node_t *current = *head;
    for (int i = 1; i < count; i++) {
        current->next = create_node(values[i]);
        if (current->next == NULL) {
            // Clean up partial list on failure
            free_linked_list(*head);
            *head = NULL;
            return MALLOC_ERROR;
        }
        current = current->next;
    }
    
    return SUCCESS;
}

// LIST_FREE: Free linked list safely
void free_linked_list(node_t *head) {
    node_t *current = head;
    node_t *temp;
    
    while (current != NULL) {
        temp = current;
        current = current->next;
        free(temp);
    }
}

// LIST_PRINT: Print linked list
int print_linked_list(node_t *head) {
    if (head == NULL) {
        printf("List is empty\n");
        return BAD_POINTER;
    }
    
    node_t *current = head;
    int count = 1;
    
    printf("\n");
    while (current != NULL) {
        printf("Value %2d = %2d\n", count, current->data);
        current = current->next;
        count++;
    }
    
    return SUCCESS;
}


// 6) --- RANDOM: Random Number Generation ---
// ------------------------------------------------------

// RANDOM_INIT: Initialize random seed
void init_random(void) {
    srandom(time(NULL) * getpid());
}

// RANDOM_FILL: Generate random integers in range
void fill_random_array(int *array, int size, int min_val, int max_val) {
    if (array == NULL || size <= 0) return;
    
    for (int i = 0; i < size; i++) {
        array[i] = (random() % (max_val - min_val + 1)) + min_val;
    }
}

// RANDOM_COUNT: Count occurrences (for histogram-style problems)
void count_values(int *data, int data_size, int *counts, int min_val, int max_val) {
    if (data == NULL || counts == NULL) return;
    
    // Initialize counts to zero
    for (int i = 0; i <= (max_val - min_val); i++) {
        counts[i] = 0;
    }
    
    // Count occurrences
    for (int i = 0; i < data_size; i++) {
        if (data[i] >= min_val && data[i] <= max_val) {
            counts[data[i] - min_val]++;
        }
    }
}


// 7) --- FILE: File I/O Templates ---
// ------------------------------------------------------

// FILE_READ: Read entire file into buffer
int read_file_to_buffer(const char *filename, char **buffer, size_t *size) {
    if (filename == NULL || buffer == NULL || size == NULL) {
        return BAD_POINTER;
    }
    
    int fd = open(filename, O_RDONLY);
    if (fd < 0) {
        perror("Failed to open file");
        return FAILURE;
    }
    
    // Get file size
    off_t file_size = lseek(fd, 0, SEEK_END);
    if (file_size < 0) {
        perror("Failed to get file size");
        close(fd);
        return FAILURE;
    }
    lseek(fd, 0, SEEK_SET);
    
    // Allocate buffer
    *buffer = malloc(file_size + 1);
    if (*buffer == NULL) {
        perror("Failed to allocate buffer");
        close(fd);
        return MALLOC_ERROR;
    }
    
    // Read file
    ssize_t bytes_read = read(fd, *buffer, file_size);
    close(fd);
    
    if (bytes_read != file_size) {
        perror("Failed to read complete file");
        free(*buffer);
        *buffer = NULL;
        return FAILURE;
    }
    
    (*buffer)[bytes_read] = '\0';
    *size = bytes_read;
    return SUCCESS;
}

// FILE_WRITE: Write buffer to file
int write_buffer_to_file(const char *filename, const char *buffer, size_t size) {
    if (filename == NULL || buffer == NULL) {
        return BAD_POINTER;
    }
    
    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        perror("Failed to create file");
        return FAILURE;
    }
    
    ssize_t bytes_written = write(fd, buffer, size);
    close(fd);
    
    if (bytes_written != (ssize_t)size) {
        perror("Failed to write complete buffer");
        return FAILURE;
    }
    
    return SUCCESS;
}


// 8) --- PROCESS: Process Management Templates ---
// ------------------------------------------------------

// PROCESS_EXEC: Execute command and wait
int execute_command(char *const argv[]) {
    if (argv == NULL || argv[0] == NULL) {
        return BAD_POINTER;
    }
    
    pid_t pid = fork();
    
    if (pid < 0) {
        perror("fork failed");
        return FAILURE;
    } else if (pid == 0) {
        // Child process
        execvp(argv[0], argv);
        perror("execvp failed");
        exit(EXIT_FAILURE);
    } else {
        // Parent process
        int status;
        if (waitpid(pid, &status, 0) < 0) {
            perror("waitpid failed");
            return FAILURE;
        }
        return WEXITSTATUS(status);
    }
}

// PROCESS_PARALLEL: Execute multiple commands in parallel
int execute_parallel_commands(char *const *commands[], int count) {
    if (commands == NULL || count <= 0) {
        return BAD_POINTER;
    }
    
    pid_t *pids = malloc(count * sizeof(pid_t));
    if (pids == NULL) {
        perror("malloc failed for pids");
        return MALLOC_ERROR;
    }
    
    // Start all processes
    for (int i = 0; i < count; i++) {
        pids[i] = fork();
        if (pids[i] < 0) {
            perror("fork failed");
            free(pids);
            return FAILURE;
        } else if (pids[i] == 0) {
            // Child process
            execvp(commands[i][0], commands[i]);
            perror("execvp failed");
            exit(EXIT_FAILURE);
        }
    }
    
    // Wait for all processes
    int result = SUCCESS;
    for (int i = 0; i < count; i++) {
        int status;
        if (waitpid(pids[i], &status, 0) < 0) {
            perror("waitpid failed");
            result = FAILURE;
        }
    }
    
    free(pids);
    return result;
}


// 9) --- THREAD: Threading Templates ---
// ------------------------------------------------------

// THREAD_GLOBALS: Global variables for threading
pthread_mutex_t global_mutex = PTHREAD_MUTEX_INITIALIZER;
bool finished = false;

// THREAD_STRUCT: Thread argument structure
typedef struct {
    int thread_id;
    void *data;
    int data_size;
} thread_args_t;

// THREAD_WORKER: Generic worker thread function
void *worker_thread(void *arg) {
    thread_args_t *args = (thread_args_t *)arg;
    
    if (args == NULL) {
        pthread_exit(NULL);
    }
    
    // Example work - modify as needed
    int *data = (int *)args->data;
    
    pthread_mutex_lock(&global_mutex);
    printf("Thread %d processing data\n", args->thread_id);
    pthread_mutex_unlock(&global_mutex);
    
    // Do work here
    for (int i = 0; i < args->data_size; i++) {
        data[i] *= 2;  // Example operation
    }
    
    pthread_exit(NULL);
}

// THREAD_CREATE: Create and manage multiple threads
int run_threaded_operation(void *data, int data_size, int num_threads) {
    if (data == NULL || data_size <= 0 || num_threads <= 0) {
        return BAD_POINTER;
    }
    
    pthread_t *threads = malloc(num_threads * sizeof(pthread_t));
    thread_args_t *args = malloc(num_threads * sizeof(thread_args_t));
    
    if (threads == NULL || args == NULL) {
        free(threads);
        free(args);
        return MALLOC_ERROR;
    }
    
    // Create threads
    for (int i = 0; i < num_threads; i++) {
        args[i].thread_id = i;
        args[i].data = data;
        args[i].data_size = data_size / num_threads;
        
        if (pthread_create(&threads[i], NULL, worker_thread, &args[i]) != 0) {
            perror("pthread_create failed");
            free(threads);
            free(args);
            return FAILURE;
        }
    }
    
    // Wait for threads to complete
    for (int i = 0; i < num_threads; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            perror("pthread_join failed");
        }
    }
    
    free(threads);
    free(args);
    return SUCCESS;
}


// 10) --- SIGNAL: Signal Handling Templates ---
// ------------------------------------------------------

// SIGNAL_GLOBALS: Global signal flags
volatile sig_atomic_t signal_received = 0;
volatile sig_atomic_t color_change = 0;

// SIGNAL_HANDLER: Signal handler function
void signal_handler(int sig) {
    switch (sig) {
        case SIGINT:
            color_change = 1;
            break;
        case SIGQUIT:
            signal_received = 1;
            break;
        default:
            break;
    }
}

// SIGNAL_SETUP: Setup signal handling
int setup_signal_handlers(void) {
    struct sigaction sa;
    
    sa.sa_handler = signal_handler;
    if (sigemptyset(&sa.sa_mask) != 0) {
        perror("sigemptyset failed");
        return FAILURE;
    }
    sa.sa_flags = SA_RESTART;
    
    if (sigaction(SIGINT, &sa, NULL) != 0) {
        perror("sigaction SIGINT failed");
        return FAILURE;
    }
    
    if (sigaction(SIGQUIT, &sa, NULL) != 0) {
        perror("sigaction SIGQUIT failed");
        return FAILURE;
    }
    
    return SUCCESS;
}


// 11) --- MAIN: Main Function Templates ---
// ------------------------------------------------------

// MAIN_BASIC: Basic main template
int main(int argc, char *argv[]) {
    int result = SUCCESS;
    int num;
    
    // Parse arguments
    result = parse_single_int(argc, argv, &num);
    if (result != SUCCESS) {
        return result;
    }
    
    // Allocate memory
    int *data = allocate_int_array(num);
    if (data == NULL) {
        return MALLOC_ERROR;
    }
    
    // Initialize random seed
    init_random();
    
    // Fill with random data
    fill_random_array(data, num, 1, 20);
    
    // Process data (example: count occurrences)
    int counts[20] = {0};
    count_values(data, num, counts, 1, 20);
    
    // Print results
    printf("\nRandom values generated: %d\n", num);
    for (int i = 0; i < 20; i++) {
        if (counts[i] > 0) {
            printf("Value %2d appeared %d times\n", i + 1, counts[i]);
        }
    }
    
    // Cleanup
    free(data);
    
    return result;
}

// MAIN_THREAD: Threading main template
int main_with_threads(int argc, char *argv[]) {
    int result = SUCCESS;
    int num;
    
    // Parse arguments
    result = parse_single_int(argc, argv, &num);
    if (result != SUCCESS) {
        return result;
    }
    
    // Setup signal handling
    if (setup_signal_handlers() != SUCCESS) {
        return FAILURE;
    }
    
    // Initialize mutex
    if (pthread_mutex_init(&global_mutex, NULL) != 0) {
        perror("pthread_mutex_init failed");
        return FAILURE;
    }
    
    // Allocate and initialize data
    int *data = allocate_int_array(num);
    if (data == NULL) {
        pthread_mutex_destroy(&global_mutex);
        return MALLOC_ERROR;
    }
    
    init_random();
    fill_random_array(data, num, 1, 100);
    
    // Run threaded operation
    result = run_threaded_operation(data, num, THREAD_COUNT);
    
    // Cleanup
    free(data);
    pthread_mutex_destroy(&global_mutex);
    
    return result;
}


// 12) --- UTIL: Utility Functions ---
// ------------------------------------------------------

// UTIL_PRINT: Print array contents
void print_array(int *array, int size, const char *label) {
    if (array == NULL || size <= 0) return;
    
    printf("\n%s:\n", label);
    for (int i = 0; i < size; i++) {
        printf("%2d ", array[i]);
        if ((i + 1) % 10 == 0) printf("\n");
    }
    if (size % 10 != 0) printf("\n");
}

// UTIL_SUM: Calculate sum of array
long calculate_sum(int *array, int size) {
    if (array == NULL || size <= 0) return 0;
    
    long sum = 0;
    for (int i = 0; i < size; i++) {
        sum += array[i];
    }
    return sum;
}

// UTIL_MINMAX: Find min and max in array
void find_min_max(int *array, int size, int *min, int *max) {
    if (array == NULL || size <= 0 || min == NULL || max == NULL) return;
    
    *min = *max = array[0];
    for (int i = 1; i < size; i++) {
        if (array[i] < *min) *min = array[i];
        if (array[i] > *max) *max = array[i];
    }
}

// ------------------------------------------------------
// QUICK SEARCH REFERENCE:
// 
// Ctrl+F these terms to find what you need:
// HEADER - file header template
// INCLUDES - standard includes and defines
// ARGS_SINGLE - parse single integer argument
// ARGS_MULTI - parse multiple arguments
// MALLOC_ARRAY - allocate array
// REALLOC - reallocate array
// LIST_STRUCT - linked list structure
// LIST_CREATE - create new node
// LIST_BUILD - build list from array
// LIST_FREE - free entire list
// LIST_PRINT - print list contents
// RANDOM_INIT - initialize random seed
// RANDOM_FILL - fill array with random numbers
// RANDOM_COUNT - count value occurrences
// FILE_READ - read file to buffer
// FILE_WRITE - write buffer to file
// PROCESS_EXEC - execute single command
// PROCESS_PARALLEL - execute multiple commands
// THREAD_GLOBALS - threading global variables
// THREAD_STRUCT - thread argument structure
// THREAD_WORKER - worker thread function
// THREAD_CREATE - create and manage threads
// SIGNAL_GLOBALS - signal handling globals
// SIGNAL_HANDLER - signal handler function
// SIGNAL_SETUP - setup signal handling
// MAIN_BASIC - basic main function template
// MAIN_THREAD - threading main template
// UTIL_PRINT - print array utility
// UTIL_SUM - calculate array sum
// UTIL_MINMAX - find min/max in array
//
// COMMON PATTERNS:
// - Always check return values and handle errors
// - Initialize errno = 0 before system calls
// - Use consistent error codes (SUCCESS, FAILURE, etc.)
// - Free all allocated memory before exit
// - Check for NULL pointers before dereferencing
// - Use mutex locks around shared data in threads
// - Initialize random seed with srandom(time(NULL) * getpid())
// - Use descriptive variable names and consistent formatting
// - Always validate input arguments
// - Use proper exit codes (EXIT_SUCCESS, EXIT_FAILURE)
// ------------------------------------------------------
