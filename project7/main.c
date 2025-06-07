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


#define SECS_PER_DAY        86400
#define SECS_PER_HOUR        3600
#define SECS_PER_MIN           60
#define HALFSECOND         500000
#define HOURS_PER_DAY          24
#define HOURS_PER_H_DAY        12

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

#define STATS_START_ROW       14
#define STATS_START_COL        1
#define NOT_CR                 1
#define CLEAR_LINE   "%-79s", ""

#define SIGEMPTY_GVAL          0
#define SIGACTION_GVAL         0
#define ATEXIT_GVAL            0
#define TCFUNC_GVAL            0
#define PTHREAD_ATTR_GVAL      0
#define PTHREAD_MUTEX_GVAL     0
#define PTHREAD_CREAT_GVAL     0
#define PTHREAD_JOIN_GVAL      0
#define TOTAL_COLORS           3
#define TIME_GVAL              0
#define RUSAGE_GVAL            0

// ------------------------------------------------------------------
// Global variables
// ------------------------------------------------------------------
pthread_mutex_t Screen_lock;
bool Finished =                     false;
bool Miltime  =                      true;
// add any other needed globals
int current_color =   COLOR_SWITCH_CASE;
struct termios og_term;

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
    if ((sigmask_rval = sigemptyset(&sa.sa_mask) || errno) != SIGEMPTY_GVAL ) {
        perror("Error masking signals");
        return result = EXIT_FAILURE;
    }
    sa.sa_flags = SA_RESTART; //no special flags

    //Register handler for interrupt signal
    errno = 0;
    siga_rval = sigaction(SIGINT, &sa, NULL);
    if (siga_rval != SIGACTION_GVAL) {
        fprintf(stderr, "Unable to pass interrupt signal for color change: %s\n",
                strerror(errno));
        return result = EXIT_FAILURE;
    }
    
    //Register handler for terminal stop signal
    errno = 0;
    siga_rval = sigaction(SIGQUIT, &sa, NULL);
    if (siga_rval != SIGACTION_GVAL) {
        fprintf(stderr, "Unable to pass quit signal to change clock display mode: %s\n",
                strerror(errno));
        return result = EXIT_FAILURE;
    }
    
    //call cleanup function through atexit()
    int atexit_rval = atexit(clean_display);
    if (atexit_rval != ATEXIT_GVAL) {
        fprintf(stderr, "atexit call to clean display failed.");
        return result = EXIT_FAILURE;
    }

    errno = 0;
    tc_rval = tcgetattr(STDIN_FILENO, &og_term);
    if (tc_rval || errno != TCFUNC_GVAL ) {
        perror("Error getting terminal attributes");
        return result = EXIT_FAILURE;
    }
    //disable echo mode
    struct termios new_term = og_term;
    new_term.c_lflag &= ~ECHO; //from man page raw mode section
    errno = 0;
    tc_rval = tcsetattr(STDIN_FILENO, TCSANOW, &new_term); //makes the change immediately via tcsanow 
    if (tc_rval || errno != TCFUNC_GVAL) {
        perror("Error disabling echo mode");
        return result = EXIT_FAILURE;
    }

    printf(HIDE_CURSOR);
    printf(CLEAR_SCREEN);

    if (pthread_mutex_init(&Screen_lock, NULL) != PTHREAD_MUTEX_GVAL) {
        fprintf(stderr, "Error initializing mutex: %s\n", strerror(errno));
        return result = EXIT_FAILURE;
    } //initialize screen lock mutex using default lock attributes
    //start time thread
    pthread_attr_init(&timeattr);
    timeattr_rval = pthread_attr_setdetachstate(&timeattr, PTHREAD_CREATE_JOINABLE);
    if (timeattr_rval != PTHREAD_ATTR_GVAL) {
        fprintf(stderr, "Error: pthread_attr_setdetachstate failed with code %d\n", timeattr_rval);
        pthread_attr_destroy(&timeattr);
        pthread_mutex_destroy(&Screen_lock);
        return result = EXIT_FAILURE;
    }
    timeattr_rval = pthread_attr_setscope(&timeattr, PTHREAD_SCOPE_SYSTEM);
    if (timeattr_rval != PTHREAD_ATTR_GVAL) {
        fprintf(stderr, "Error: pthrtead_attr_setscope failed with code %d\n", timeattr_rval);
        pthread_attr_destroy(&timeattr);
        pthread_mutex_destroy(&Screen_lock);
        return result = EXIT_FAILURE;
    }
    time_rval = pthread_create(&time_thread, &timeattr, mil_time, NULL);
    if (time_rval != PTHREAD_CREAT_GVAL) {
        fprintf(stderr, "Error creating time thread.");
        pthread_attr_destroy(&timeattr);
        pthread_mutex_destroy(&Screen_lock);
        return result = EXIT_FAILURE;
    }
    pthread_attr_init(&statsattr);
    statsattr_rval = pthread_attr_setdetachstate(&statsattr, PTHREAD_CREATE_JOINABLE);
    if (statsattr_rval != PTHREAD_ATTR_GVAL) {
        fprintf(stderr, "Error: pthread_attr_setdetachstate failed with code %d\n", statsattr_rval);
        pthread_attr_destroy(&statsattr);
        pthread_mutex_destroy(&Screen_lock);
        return result = EXIT_FAILURE;
    }
    statsattr_rval = pthread_attr_setscope(&statsattr, PTHREAD_SCOPE_SYSTEM);
    if (statsattr_rval != PTHREAD_ATTR_GVAL) {
        fprintf(stderr, "Error: pthread_attr_setscope failed wit5h code %d\n", statsattr_rval);
        pthread_attr_destroy(&statsattr);
        pthread_mutex_destroy(&Screen_lock);
        return result = EXIT_FAILURE;
    }
    stats_rval = pthread_create(&stats_thread, &statsattr, clock_stats, NULL);
    if (stats_rval != PTHREAD_CREAT_GVAL ){
        fprintf(stderr, "Error creating statistics thread.");
        pthread_attr_destroy(&statsattr);
        pthread_mutex_destroy(&Screen_lock);
        return result = EXIT_FAILURE;
    }
    pthread_attr_destroy(&timeattr);
    pthread_attr_destroy(&statsattr);
    
    //lock the tterminal
    pthread_mutex_lock(&Screen_lock);
    //display user prompt for interrupts

    int row = PROMPT_START_ROW;
    int col = PROMPT_START_COL;
    printf(MOVE_CURSOR, row, col);

    printf("Press Ctrl-C to change clock color.\n");
    printf("Press Ctrl-\\ to change clock time format.\n");
    printf("Press CR to Exit.\n");

    fflush(stdout);
    //unlcok the terminal
    pthread_mutex_unlock(&Screen_lock);
    //wait for CR
    int c;
    while ((c = getchar()) != EOF) {
        if (c == '\n' || c == '\r') {
            break; //if CR then done
        }
    }

    //at this point the user has hit CR
    Finished = true;

    //wait for threadds to finish
    timejoin_rval = pthread_join(time_thread, NULL);
    if (timejoin_rval != PTHREAD_JOIN_GVAL) {
        fprintf(stderr, "Error: pthread_join failed with code %d\n", timejoin_rval);
        return result = EXIT_FAILURE;
    }
    statsjoin_rval = pthread_join(stats_thread, NULL);
    if (statsjoin_rval != PTHREAD_JOIN_GVAL) {
        fprintf(stderr, "Error: pthread_join failed with code %d\n", statsjoin_rval);
        return result = EXIT_FAILURE;
    }
    printf("made it to end of main function.\n");
    pthread_mutex_destroy(&Screen_lock);
    return result = EXIT_SUCCESS;

    //now cleanup function will run and program will end safely
    

}//end main


// ------------------------------------------------------------------
// (Other function definitions)
// ------------------------------------------------------------------

void signal_handler(int sig){
        //wrap around to each index upon press
        if (sig == SIGINT) {
            current_color = (current_color +1) % TOTAL_COLORS;
        } else if (sig == SIGQUIT) {
        Miltime = !Miltime;
        }
    }

void *mil_time(void * arg) {
    tzset();
    //call tzset to initalize time zone information
    
    while (!Finished) {   
        
        errno = 0;
        long epoch_secs = time(NULL);
        if (errno != TIME_GVAL) {
            perror("Error calling time function");
            pthread_exit(NULL);
        }
        
        long adjusted_time = epoch_secs - timezone;
        adjusted_time += (daylight * SECS_PER_HOUR); //always apply DST

        int hours = (adjusted_time % SECS_PER_DAY) / SECS_PER_HOUR;
        int min = (adjusted_time % SECS_PER_HOUR) / SECS_PER_MIN;


        //wrap around for 24 hours cycle from total amount of hours
        if (hours >= HOURS_PER_DAY) {
            hours -= HOURS_PER_DAY;
        } else if (hours <0) {
            hours += HOURS_PER_DAY;
        }

        //handling mil time switch
        int hour = hours;
        if (!Miltime) {
            if (hours == 0) {
                hour = HOURS_PER_H_DAY;
            } else if (hours > HOURS_PER_H_DAY) {
                hour = hours - HOURS_PER_H_DAY;
            }
        }

        pthread_mutex_lock(&Screen_lock);
        switch(current_color) {
            case RED_COLOR : printf(RED);
            break;
            case GREEN_COLOR: printf(GREEN);
            break;
            default:
            printf(DEFAULT_COLOR);
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
        if (usage_rval || errno != RUSAGE_GVAL) {
            perror("Issues getting usage stats for calling process.");
            pthread_exit(NULL);
        }
        pthread_mutex_lock(&Screen_lock);
        printf(MOVE_CURSOR, STATS_START_ROW, STATS_START_COL);
        printf(CLEAR_LINE); //to avoid overprinting lines
        printf(DEFAULT_COLOR);
        printf(MOVE_CURSOR, STATS_START_ROW, STATS_START_COL);
        printf("User CPU time\t : %ld sec., %ld microsec.", usage.ru_utime.tv_sec, usage.ru_utime.tv_usec);
        printf(MOVE_CURSOR, STATS_START_ROW+1, STATS_START_COL);
        printf(CLEAR_LINE);
        printf(DEFAULT_COLOR);
        printf(MOVE_CURSOR, STATS_START_ROW+1, STATS_START_COL);
        printf("System CPU time  : %ld sec., %ld microsec.", usage.ru_stime.tv_sec, usage.ru_stime.tv_usec);
        fflush(stdout);
        pthread_mutex_unlock(&Screen_lock);
        
        usleep(HALFSECOND);

    }
    pthread_exit(NULL);
}

void clean_display(void) {
    printf("in cleanup function");
    int row = CLEANUP_ROW_TERM;
    int col = CLEANUP_COL_TERM;
    //reset display color back to default
    current_color = START_COLOR;
    //turn cursor back on
    printf(SHOW_CURSOR);
    //turn back on keyboard input echoing
    
    int tcg_rval = tcgetattr(STDIN_FILENO, &og_term);
    if (tcg_rval != 0 ) {
        perror("Error getting terminal attributes");
    }
    //enable echo mode
    og_term.c_lflag |= ECHO; //from man page raw mode section
    
    int tcs_rval = tcsetattr(STDIN_FILENO, TCSANOW, &og_term); //makes the change immediately via tcsanow 
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
