// ----------------------------------------------------------------------
// File: main.c
//
// Name: Al Shaffer & Paul Clark
//
// Description: This is the main program for a simple Yahtzee game.
//
// Syntax: ./yahtzee [any other user inputs are ignored]
// ----------------------------------------------------------------------

#include <stdlib.h>
#include "play.h"
#include "screen.h"
#include "score.h"


// **************************************************************************
// *********************************  MAIN **********************************
// **************************************************************************
int main(void)
{

    // Initialize the screen module
    screen_init();

    // Initialize the score module
    score_reset();

    // Play the game
    play_yahtzee();

    // Reset the screen as it was before game started
    screen_reset();

    // Display the final score sheet
    score_display_final();

    return EXIT_SUCCESS;

} // end main

// end main.c
