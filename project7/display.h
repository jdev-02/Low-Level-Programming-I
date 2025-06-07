// ------------------------------------------------------------------
// File: display.h
//
// Name: Al Shaffer & Paul Clark
//
// Description:
//     This is the header file for the DISPLAY module, which is used
//     to display a large clock.
// ------------------------------------------------------------------

#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#define DIGIT_WIDTH    9    // # characters that make up the width
#define DIGIT_HEIGHT  10    // # characters that make up the height
#define DIGIT_SPACING  2    // # spaces between displayed digits

// ------------------------------------------------------------------
// Function:
//     display_time
// Inputs:
//     row   The terminal row to start the clock display
//     col   The terminal col to start the clock display
//     hours The hour to display in the clock ("HH")
//     mins  The minutes to display in the clock ("MM")
// Description:
//     This function displays the input time in a "HH:MM" format at
//     the position (row,col) of the terminal with large "numbers"
//     that are each DIGIT_WIDTH wide and DIGIT_HEIGHT tall.
// ------------------------------------------------------------------
extern void display_time(
    const unsigned int row,      // The terminal row to start the clock
    const unsigned int col,      // The terminal col to start the clock
    const unsigned int hours,    // The hours to display "HH"
    const unsigned int mins);    // The minutes to display "MM"

#endif

//end display.h
