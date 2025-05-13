// ----------------------------------------------------------------------
// File: screen.c
//
// Name: Al Shaffer & Paul Clark
//
// Description:
//     This module sets the foreground and background colors of the
//     terminal, clears the screen, and resets the terminal back to its
//     original state.
// ----------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include "screen.h"

#define YAHTZEE_ROWS         24
#define YAHTZEE_COLS         80
#define HOME_ROW              1
#define HOME_COL              1
#define BACKGROUND_GREEN     42
#define MOVE_CURSOR          "\x1b[%i;%iH"  // row, column
#define CLEAR_SCREEN         "\x1b[2J"
#define SET_FOREGROUND_COLOR "\x1b[%i"
#define SET_BOTH_COLORS      "\x1b[%i;%im"  // background, foreground
#define COLOR_RESET          "\x1b[0m"

#define NEW_SCREEN() \
        printf(CLEAR_SCREEN); \
        fflush(stdout);

// ***********************************************************************
// **************************** GLOBAL VARIABLES *************************
// ***********************************************************************

static bool Screen_initialized = false;


// ***********************************************************************
// *************************** INTERNAL FUNCTIONS ************************
// ***********************************************************************

static void set_colors(void)
{
    printf(SET_BOTH_COLORS, BACKGROUND_GREEN, WHITE_TEXT);
    fflush(stdout);
}//end set_colors


// ***********************************************************************
// *************************** EXTERNAL FUNCTIONS ************************
// ***********************************************************************

// ---------------------------------------------------------------------
// Function
//     screen_clear
// Inputs
//     none
// Outputs
//     none
// Description
//     Clears the terminal screen
// ---------------------------------------------------------------------
void screen_clear(void)
{
    NEW_SCREEN();
}//end screen_clear


// ---------------------------------------------------------------------
// Function
//     screen_reset
// Inputs
//     none
// Outputs
//     none
// Description
//     This function clears the screen, and then moves the cursor to the 
//     top lefthand corner of the screen
// ---------------------------------------------------------------------
void screen_reset(void)
{
    // Reset the color scheme
    printf(COLOR_RESET);

    // Clear the screen and move cursor to the top
    NEW_SCREEN();
    printf(MOVE_CURSOR, HOME_ROW, HOME_COL);
    fflush(stdout);

}//end screen_reset


// ---------------------------------------------------------------------
// Function
//     screen_init
// Inputs
//     none
// Outputs
//     none
// Description
//     This function verifies that the terminal has the minimal
//     dimensions for the game, sets the color scheme, and then
//     clears the terminal in preparation for starting the game.
//     This function is intended to be used once. If it is called more
//     than once then the program will terminate.
// ---------------------------------------------------------------------
void screen_init(void)
{
    int result;
    struct winsize term;

    // Make sure this hasn't been called before
    if (Screen_initialized) {
        screen_reset();
        printf("\n\nError: Screen module initialized twice.\n\n");
        exit(EXIT_FAILURE);
    }

    // Get the current dimensions of the terminal, and the current
    // state of the terminal configuration so it can be restore later.
    errno = 0;
    result = ioctl(STDOUT_FILENO, TIOCGWINSZ, &term);
    if (result < 0) {
        perror("Unable to determine terminal size");
        exit(EXIT_FAILURE);
    }

    // Make sure the terminal is large enough for the game.
    if ((term.ws_row < YAHTZEE_ROWS) || (term.ws_col < YAHTZEE_COLS)) {
        printf("Error: the terminal must have a miminum of %i rows "
               "and %i columns", YAHTZEE_ROWS, YAHTZEE_COLS);
        exit(EXIT_FAILURE);
    }

    // Set color scheme and initialize the screen
    set_colors();
    NEW_SCREEN();

    // Remember that we've done this
    Screen_initialized = true;

}//end screen_init


// ---------------------------------------------------------------------
// Function
//     screen_cursor
// Inputs
//     row
//         The row where the cursor should go
//     col
//         The column where the cursor should go
// Outputs
//     none
// Description
//     This function moves the cursor to the given (row, col) address
//     of the terminal. It does not check whether the inputs make sense
//     for the current dimensions of the terminal.
// ---------------------------------------------------------------------
void screen_cursor(const int row, const int col)
{
    printf(MOVE_CURSOR, row, col);
}//end screen_cursor


// ---------------------------------------------------------------------
// Function
//     screen_text_color
// Inputs
//     color
//         The color terminal text should be.
// Outputs
//     none
// Description
//     This function changes the color that will be used for all
//     displaying of data to the terminal (until it is changed again).
// ---------------------------------------------------------------------
void screen_text_color(const int color)
{
    printf(SET_BOTH_COLORS, BACKGROUND_GREEN, color);
}//end screen_text_color
