// ------------------------------------------------------------------
// File: main.c
//
// Name: Al Shaffer & Paul Clark & Jonathan Goohs
//
// Description:
//     This file contains the main entry point into a program that
//     generates two additional threads: 1) one thread to display a
//     large clock of the current time; 2) one thread to display CPU
//     resources usage. The program also responds to signals during
//     execution.
//
// Syntax:
//     ./clock
//     The program ignores any inputs provided by the user.
//
// Resources
// 1. tcsetattr man page
// 2. sigaction man page
// 3. getchar man page
// 4. tzsetr man page
// 5. stackoverflow.com/questions/50227212/how-to-get-epoch-day-and-time-in-c-for-even-different-time-zone
// ------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <pthread.h>
#include <termios.h> //for tcgetattr
#include <sys/ioctl.h>
#include <signal.h> //for siigaction
#include <string.h>
#include <unistd.h> //for posix os function - STDIN_FILENO
#include <time.h> //for tzset() and time()
#include <sys/resource.h> //for getrusage
#include "display.h"


#define SECS_PER_DAY       86400
#define SECS_PER_HOUR       3600
#define SECS_PER_MIN          60
#define HALFSECOND        500000

#define MOVE_CURSOR "\x1b[%d;%dH"
#define HIDE_CURSOR   "\x1b[?25l"
#define SHOW_CURSOR   "\x1b[?25h"
#define CLEAR_SCREEN    "\x1b[2J"

#define DISPLAY_START_ROW      2
#define DISPLAY_START_COL      1       


#define RED            "\x1b[31m"
#define GREEN          "\x1b[32m"
#define DEFAULT_COLOR   "\x1b[0m"
#define START_COLOR            0
#define RED_COLOR              1
#define GREEN_COLOR            2
#define RESET_COLOR            3

#define SIG_HANDLE_ER          1
#define CLEANUP_ROW_TERM       1
#define CLEANUP_COL_TERM       1

#define COLOR_SWITCH_CASE      0
#define MIL_CIV_SWITCH_CASE    0

#define PROMPT_START_ROW      18
#define PROMPT_START_COL       1

#define STATS_START_ROW       12
#define STATS_START_COL        1
#define NOT_CR                 1


// ------------------------------------------------------------------
// Global variables
// ------------------------------------------------------------------
pthread_mutex_t Screen_lock;
bool Finished =                     false;
bool Miltime  =                      true;
// add any other needed globals
int current_color_i =   COLOR_SWITCH_CASE;
int mil_civ_display = MIL_CIV_SWITCH_CASE;
//set by tzset()
extern long timezone;
extern int daylight;


// ------------------------------------------------------------------
// Function prototypes
// ------------------------------------------------------------------
void *mil_time(void * arg);
void *clock_stats(void *arg);
void signal_handler(int sig);
void clean_display(void);

// ********************************************************************
// ****************************** M A I N *****************************
// ********************************************************************
int main(void)
{
    int result = EXIT_SUCCESS;
    struct sigaction sa;
    int siga_rval;
    int sigmask_rval;
    struct termios term_dis; //for disabling echo commands
    int tc_rval;
    
    pthread_t time_thread;
    int time_rval;
    pthread_attr_t timeattr;
    int timeattr_rval;
    int timejoin_rval;

    pthread_t stats_thread;
    int stats_rval;
    pthread_attr_t statsattr;
    int statsattr_rval;
    int statsjoin_rval;

    // Prepare signal action settings
    sa.sa_handler = signal_handler; //set the hanlder function
    errno = 0;
    if ((sigmask_rval = sigemptyset(&sa.sa_mask) || errno) != 0  ) {
        perror("Error masking signals");
        return result = EXIT_FAILURE;
    }
    sa.sa_flags = SA_RESTART; //no special flags

    //Register handler for interrupt signal
    errno = 0;
    siga_rval = sigaction(SIGINT, &sa, NULL);
    if (siga_rval != 0) {
        fprintf(stderr, "Unable to pass interrupt signal for color change: %s\n",
                strerror(errno));
        return result = EXIT_FAILURE;
    }
    
    //Register handler for terminal stop signal
    errno = 0;
    siga_rval = sigaction(SIGQUIT, &sa, NULL);
    if (siga_rval != 0) {
        fprintf(stderr, "Unable to pass quit signal to change clock display mode: %s\n",
                strerror(errno));
        return result = EXIT_FAILURE;
    }
    
    //call cleanup function through atexit()
    int atexit_rval = atexit(clean_display);
    if (atexit_rval != 0) {
        fprintf(stderr, "atexit call to clean display failed.");
        return result = EXIT_FAILURE;
    }

    errno = 0;
    tc_rval = tcgetattr(STDIN_FILENO, &term_dis);
    if (tc_rval || errno != 0 ) {
        perror("Error getting terminal attributes");
        return result = EXIT_FAILURE;
    }
    //disable echo mode
    term_dis.c_lflag |= ~ECHO; //from man page raw mode section
    errno = 0;
    tc_rval = tcsetattr(STDIN_FILENO, TCSANOW, &term_dis); //makes the change immediately via tcsanow 
    if (tc_rval || errno != 0) {
        perror("Error disabling echo mode");
        return result = EXIT_FAILURE;
    }

    printf(HIDE_CURSOR);
    printf(CLEAR_SCREEN);

    pthread_mutex_init(&Screen_lock, NULL); //initialize screen lock mutex using default lock attributes
    //start time thread
    pthread_attr_init(&timeattr);
    timeattr_rval = pthread_attr_setdetachstate(&timeattr, PTHREAD_CREATE_JOINABLE);
    if (timeattr_rval != 0) {
        fprintf(stderr, "Error: pthread_attr_setdetachstate failed with code %d\n", timeattr_rval);
        return result = EXIT_FAILURE;
    }
    timeattr_rval = pthread_attr_setscope(&timeattr, PTHREAD_SCOPE_SYSTEM);
    if (timeattr_rval != 0) {
        fprintf(stderr, "Error: pthrtead_attr_setscope failed with code %d\n", timeattr_rval);
        return result = EXIT_FAILURE;
    }
    time_rval = pthread_create(&time_thread, &timeattr, mil_time, NULL);
    if (time_rval != 0) {
        fprintf(stderr, "Error creating time thread.");
        return result = EXIT_FAILURE;
    }
    pthread_attr_init(&statsattr);
    statsattr_rval = pthread_attr_setdetachstate(&statsattr, PTHREAD_CREATE_JOINABLE);
    if (statsattr_rval != 0) {
        fprintf(stderr, "Error: pthread_attr_setdetachstate failed with code %d\n", statsattr_rval);
        return result = EXIT_FAILURE;
    }
    statsattr_rval = pthread_attr_setscope(&statsattr, PTHREAD_SCOPE_SYSTEM);
    if (statsattr_rval != 0) {
        fprintf(stderr, "Error: pthread_attr_setscope failed wit5h code %d\n", statsattr_rval);
        return result = EXIT_FAILURE;
    }
    stats_rval = pthread_create(&stats_thread, &statsattr, clock_stats, NULL);
    if (stats_rval != 0 ){
        fprintf(stderr, "Error creating statistics thread.");
        return result = EXIT_FAILURE;
    }
    
    //lock the tterminal
    pthread_mutex_lock(&Screen_lock);
    //display user prompt for interrupts
    int row = PROMPT_START_ROW;
    int col = PROMPT_START_COL;
    printf(MOVE_CURSOR, row, col);
    printf("Press Ctrl-C to change clock color.\n");
    printf("Press Ctrl-\\ to change clock time format.\n");
    printf("Press CR to Exit.\n");
    //unlcok the terminal
    pthread_mutex_unlock(&Screen_lock);
    //wait for CR
    int c;
    while (NOT_CR) {
        errno = 0;
        c = getchar();
        //error handle via fgetc man page
        if (c == EOF  || errno != 0 ) {
            perror("get char error\n");
            return result = EXIT_FAILURE;
        }
        if (c== '\n' || c == '\r') {
            break;
        }
    }
    
    //at this point the user has hit CR
    Finished = true;

    //wait for threadds to finish
    timejoin_rval = pthread_join(time_thread, NULL);
    if (timejoin_rval !=0) {
        fprintf(stderr, "Error: pthread_join failed with code %d\n", timejoin_rval);
        return result = EXIT_FAILURE;
    }
    statsjoin_rval = pthread_join(stats_thread, NULL);
    if (statsjoin_rval != 0) {
        fprintf(stderr, "Error: pthread_join failed with code %d\n", statsjoin_rval);
        return result = EXIT_FAILURE;
    }
    printf("made it to end of main function.\n");
    pthread_mutex_destroy(&Screen_lock);
    return result = EXIT_SUCCESS;
    fflush(stdout);

    
    //now cleanup function will run and program will end safely
    

}//end main


// ------------------------------------------------------------------
// (Other function definitions)
// ------------------------------------------------------------------

void signal_handler(int sig){
        
        
        //wrap around to each index upon press
        if (sig == SIGINT) {
            current_color_i = (current_color_i +1) % 3;
        }
        if (sig == SIGQUIT) {
        Miltime = !Miltime;
        }
    }

void *mil_time(void * arg) {
    tzset();
    //call tzset to initalize time zone information
    
    while (!Finished) {   
        
        errno = 0;
        long epoch_secs = time(NULL);
        if (errno != 0) {
            perror("Error calling time function");
            pthread_exit(NULL);
        }
        
        long adjusted_time = epoch_secs - timezone;
        int hours = (adjusted_time % SECS_PER_DAY) / SECS_PER_HOUR;
        int min = (adjusted_time % SECS_PER_HOUR) / SECS_PER_MIN;

        hours += daylight; //continue applynig daylight savings
        //handling mil time switch
        int hour;
        if (!Miltime) {
            if (hours == 0) {
                hour = 12;
            } else if (hours > 12) {
                hour = hours - 12;
            }
        }

        pthread_mutex_lock(&Screen_lock);
        switch(current_color_i) {
            case RED_COLOR : printf(RED);
            break;
            case GREEN_COLOR: printf(GREEN);
            break;
        }
        display_time(DISPLAY_START_ROW, DISPLAY_START_COL, hour, min);

        fflush(stdout);
        pthread_mutex_unlock(&Screen_lock);

        //loop with a sleep or wait for 0.5 seconds using usleep(500000) in macro HALFSECONDy
        usleep(HALFSECOND);
    }
    pthread_exit(NULL);
    
}

void *clock_stats(void *arg) {
    while (Finished != true) {
        struct rusage usage; //declare struct for getusage call
        int usage_rval = getrusage(RUSAGE_SELF, &usage);
        errno = 0;
        if (usage_rval || errno != 0) {
            perror("Issues getting usage stats for calling process.");
            pthread_exit(NULL);
        }
        pthread_mutex_lock(&Screen_lock);
        printf("\n\nUser CPU time\t : %ld sec., %ld microsec.\n", usage.ru_utime.tv_sec, usage.ru_utime.tv_usec);
        printf("System CPU time  : %ld sec., %ld microsec.", usage.ru_stime.tv_sec, usage.ru_stime.tv_usec);
        fflush(stdout);
        pthread_mutex_unlock(&Screen_lock);
        
        usleep(HALFSECOND);

    }
    pthread_exit(NULL);
}

void clean_display(void) {
    printf("in cleanup function");
    struct termios term;
    int row = CLEANUP_ROW_TERM;
    int col = CLEANUP_COL_TERM;
    //reset display color back to default
    current_color_i = START_COLOR;
    //turn cursor back on
    printf(SHOW_CURSOR);
    //turn back on keyboard input echoing
    
    int tcg_rval = tcgetattr(STDIN_FILENO, &term);
    if (tcg_rval != 0 ) {
        perror("Error getting terminal attributes");
    }
    //enable echo mode
    term.c_lflag |= ECHO; //from man page raw mode section
    
    int tcs_rval = tcsetattr(STDIN_FILENO, TCSANOW, &term); //makes the change immediately via tcsanow 
    if (tcs_rval != 0) {
        perror("Error enabling echo mode");
    }

    //display the cursor below the program display, at the left hand column of the terminal
    printf(CLEAR_SCREEN);
    printf(MOVE_CURSOR, row, col);
    
    fflush(stdout);
    fflush(stderr);
}

//end main.c
