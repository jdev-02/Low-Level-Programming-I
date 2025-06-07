// ---------------------------------------------------------------------
// File: display.c
//
// Name: Al Shaffer & Paul Clark
//
// Description:
//     This module displays the given hours and minutes of the day in
//     hh:mm format in large "text".
// ---------------------------------------------------------------------

#include <stdio.h>
#include <stdbool.h>
#include "display.h"

#define MOVE_CURSOR   "\x1b[%d;%dH"
#define BASE_10       10

#define HOUR2_OFFSET  (DIGIT_WIDTH + DIGIT_SPACING)
#define COLON_OFFSET  (2 * (DIGIT_WIDTH + DIGIT_SPACING))
#define MIN1_OFFSET   (3 * (DIGIT_WIDTH + DIGIT_SPACING))
#define MIN2_OFFSET   (4 * (DIGIT_WIDTH + DIGIT_SPACING))

#define HOURS_PER_DAY    24
#define MINUTES_PER_HOUR 60
#define COLON            ':'

// ---------------------------------------------------------------------
// Name:
//     display_num
// Inputs:
//     row
//         The terminal row from which to start the display of the
//         given number in large "text". This is the upper-most row
//         of the number.
//     col
//         The terminal column from which to start the display of the
//         given number in large "text". This is the left-most edge of
//         the number.
//     num
//         The numerical value to display. This is expected to be a
//         number between 0 and 9 (inclusive) or the ":" seperator.
// Outputs:
//     N/A
// Description:
//     This function displays a digit (or colon) using large "text"
//     that is made up of individual ASCII values. Each larger digit
//     is DIGIT_WIDTH characters high and DIGIT_HEIGHT tall. The input
//     (x,y) represents the upper-left-most corner of the larger text.
//     If an an illegal value in passed in the input num, then the
//     larger text will be a full block of characters to represent an
//     illegal value.
// ---------------------------------------------------------------------
static void display_num(const int row, const int col, unsigned int num)
{
    unsigned int new_row = row;

    switch (num) {    // cases for each of the clock display digits
        case (0):
            printf(MOVE_CURSOR " XXXXXXX ", new_row++, col);
            printf(MOVE_CURSOR "XXXXXXXXX", new_row++, col);
            printf(MOVE_CURSOR "XXX   XXX", new_row++, col);
            printf(MOVE_CURSOR "XXX   XXX", new_row++, col);
            printf(MOVE_CURSOR "XXX   XXX", new_row++, col);
            printf(MOVE_CURSOR "XXX   XXX", new_row++, col);
            printf(MOVE_CURSOR "XXX   XXX", new_row++, col);
            printf(MOVE_CURSOR "XXX   XXX", new_row++, col);
            printf(MOVE_CURSOR "XXXXXXXXX", new_row++, col);
            printf(MOVE_CURSOR " XXXXXXX ", new_row  , col);
            break;
        case (1):
            printf(MOVE_CURSOR "   XXX   ", new_row++, col);
            printf(MOVE_CURSOR "  XXXX   ", new_row++, col);
            printf(MOVE_CURSOR "   XXX   ", new_row++, col);
            printf(MOVE_CURSOR "   XXX   ", new_row++, col);
            printf(MOVE_CURSOR "   XXX   ", new_row++, col);
            printf(MOVE_CURSOR "   XXX   ", new_row++, col);
            printf(MOVE_CURSOR "   XXX   ", new_row++, col);
            printf(MOVE_CURSOR "   XXX   ", new_row++, col);
            printf(MOVE_CURSOR " XXXXXXX ", new_row++, col);
            printf(MOVE_CURSOR "XXXXXXXXX", new_row  , col);
            break;
        case (2):
            printf(MOVE_CURSOR " XXXXXX  ", new_row++, col);
            printf(MOVE_CURSOR "XXXXXXXX ", new_row++, col);
            printf(MOVE_CURSOR " XX  XXX ", new_row++, col);
            printf(MOVE_CURSOR "    XXX  ", new_row++, col);
            printf(MOVE_CURSOR "   XXX   ", new_row++, col);
            printf(MOVE_CURSOR "  XXX    ", new_row++, col);
            printf(MOVE_CURSOR " XXX     ", new_row++, col);
            printf(MOVE_CURSOR "XXX      ", new_row++, col);
            printf(MOVE_CURSOR "XXXXXXXXX", new_row++, col);
            printf(MOVE_CURSOR "XXXXXXXXX", new_row,   col);
            break;
        case (3):
            printf(MOVE_CURSOR " XXXXXXX ", new_row++, col);
            printf(MOVE_CURSOR "XXXXXXXXX", new_row++, col);
            printf(MOVE_CURSOR "      XXX", new_row++, col);
            printf(MOVE_CURSOR "      XXX", new_row++, col);
            printf(MOVE_CURSOR "  XXXXXXX", new_row++, col);
            printf(MOVE_CURSOR "  XXXXXXX", new_row++, col);
            printf(MOVE_CURSOR "      XXX", new_row++, col);
            printf(MOVE_CURSOR "      XXX", new_row++, col);
            printf(MOVE_CURSOR "XXXXXXXXX", new_row++, col);
            printf(MOVE_CURSOR " XXXXXXX ", new_row,   col);
            break;
        case (4):
            printf(MOVE_CURSOR "XXX      ", new_row++, col);
            printf(MOVE_CURSOR "XXX      ", new_row++, col);
            printf(MOVE_CURSOR "XXX  XXX ", new_row++, col);
            printf(MOVE_CURSOR "XXX  XXX ", new_row++, col);
            printf(MOVE_CURSOR "XXXXXXXXX", new_row++, col);
            printf(MOVE_CURSOR "XXXXXXXXX", new_row++, col);
            printf(MOVE_CURSOR "     XXX ", new_row++, col);
            printf(MOVE_CURSOR "     XXX ", new_row++, col);
            printf(MOVE_CURSOR "     XXX ", new_row++, col);
            printf(MOVE_CURSOR "     XXX ", new_row,   col);
            break;
        case (5):
            printf(MOVE_CURSOR "XXXXXXXXX", new_row++, col);
            printf(MOVE_CURSOR "XXXXXXXXX", new_row++, col);
            printf(MOVE_CURSOR "XXX      ", new_row++, col);
            printf(MOVE_CURSOR "XXX      ", new_row++, col);
            printf(MOVE_CURSOR "XXXXXXXX ", new_row++, col);
            printf(MOVE_CURSOR "XXXXXXXXX", new_row++, col);
            printf(MOVE_CURSOR "      XXX", new_row++, col);
            printf(MOVE_CURSOR "      XXX", new_row++, col);
            printf(MOVE_CURSOR "XXXXXXXXX", new_row++, col);
            printf(MOVE_CURSOR "XXXXXXXX ", new_row,   col);
            break;
        case (6):
            printf(MOVE_CURSOR " XXXXXXX ", new_row++, col);
            printf(MOVE_CURSOR "XXXXXXXXX", new_row++, col);
            printf(MOVE_CURSOR "XXX      ", new_row++, col);
            printf(MOVE_CURSOR "XXX      ", new_row++, col);
            printf(MOVE_CURSOR "XXXXXXX  ", new_row++, col);
            printf(MOVE_CURSOR "XXXXXXXX ", new_row++, col);
            printf(MOVE_CURSOR "XXX   XXX", new_row++, col);
            printf(MOVE_CURSOR "XXX   XXX", new_row++, col);
            printf(MOVE_CURSOR "XXXXXXXXX", new_row++, col);
            printf(MOVE_CURSOR " XXXXXXX ", new_row,   col);
            break;
        case (7):
            printf(MOVE_CURSOR "XXXXXXXXX", new_row++, col);
            printf(MOVE_CURSOR "XXXXXXXXX", new_row++, col);
            printf(MOVE_CURSOR "      XXX", new_row++, col);
            printf(MOVE_CURSOR "     XXX ", new_row++, col);
            printf(MOVE_CURSOR "    XXX  ", new_row++, col);
            printf(MOVE_CURSOR "   XXX   ", new_row++, col);
            printf(MOVE_CURSOR "  XXX    ", new_row++, col);
            printf(MOVE_CURSOR " XXX     ", new_row++, col);
            printf(MOVE_CURSOR "XXX      ", new_row++, col);
            printf(MOVE_CURSOR "XXX      ", new_row,   col);
            break;
        case (8):
            printf(MOVE_CURSOR " XXXXXXX ", new_row++, col);
            printf(MOVE_CURSOR "XXXXXXXXX", new_row++, col);
            printf(MOVE_CURSOR "XXX   XXX", new_row++, col);
            printf(MOVE_CURSOR "XXX   XXX", new_row++, col);
            printf(MOVE_CURSOR " XXXXXXX ", new_row++, col);
            printf(MOVE_CURSOR " XXXXXXX ", new_row++, col);
            printf(MOVE_CURSOR "XXX   XXX", new_row++, col);
            printf(MOVE_CURSOR "XXX   XXX", new_row++, col);
            printf(MOVE_CURSOR "XXXXXXXXX", new_row++, col);
            printf(MOVE_CURSOR " XXXXXXX ", new_row,   col);
            break;
        case (9):
            printf(MOVE_CURSOR " XXXXXXX ", new_row++, col);
            printf(MOVE_CURSOR "XXXXXXXXX", new_row++, col);
            printf(MOVE_CURSOR "XXX   XXX", new_row++, col);
            printf(MOVE_CURSOR "XXX   XXX", new_row++, col);
            printf(MOVE_CURSOR " XXXXXXXX", new_row++, col);
            printf(MOVE_CURSOR "  XXXXXXX", new_row++, col);
            printf(MOVE_CURSOR "     XXX ", new_row++, col);
            printf(MOVE_CURSOR "    XXX  ", new_row++, col);
            printf(MOVE_CURSOR "   XXX   ", new_row++, col);
            printf(MOVE_CURSOR "  XXX    ", new_row,   col);
            break;
        case (COLON):    // colon for non-mil time
            printf(MOVE_CURSOR "         ", new_row++, col);
            printf(MOVE_CURSOR "         ", new_row++, col);
            printf(MOVE_CURSOR "   XXX   ", new_row++, col);
            printf(MOVE_CURSOR "   XXX   ", new_row++, col);
            printf(MOVE_CURSOR "         ", new_row++, col);
            printf(MOVE_CURSOR "         ", new_row++, col);
            printf(MOVE_CURSOR "   XXX   ", new_row++, col);
            printf(MOVE_CURSOR "   XXX   ", new_row++, col);
            printf(MOVE_CURSOR "         ", new_row++, col);
            printf(MOVE_CURSOR "         ", new_row,   col);
            break;
        default:    // default blank space
            printf(MOVE_CURSOR "         ", new_row++, col);
            printf(MOVE_CURSOR "         ", new_row++, col);
            printf(MOVE_CURSOR "         ", new_row++, col);
            printf(MOVE_CURSOR "         ", new_row++, col);
            printf(MOVE_CURSOR "         ", new_row++, col);
            printf(MOVE_CURSOR "         ", new_row++, col);
            printf(MOVE_CURSOR "         ", new_row++, col);
            printf(MOVE_CURSOR "         ", new_row++, col);
            printf(MOVE_CURSOR "         ", new_row++, col);
            printf(MOVE_CURSOR "         ", new_row,   col);
            break;
    }

    fflush(stdout);

}//end display_num


// ---------------------------------------------------------------------
// Name:
//     display_time
// Inputs:
//     row
//         The terminal row from which to start the display of the
//         given number in large "text". This is the upper-most row
//         of the number.
//     col
//         The terminal column from which to start the display of the
//         given number in large "text". This is the left-most edge of
//         the number.
//     hours
//         The current hour in 24-hour or 12-hour format.
//     mins
//         The current minute within the current hour.
// Outputs:
//     N/A
// Description:
//     This is an external function to be used to display the input
//     hours and mins in HH:MM format on the screen in a large format.
// ---------------------------------------------------------------------
void display_time(const unsigned int row,
                  const unsigned int col,
                  const unsigned int hours,
                  const unsigned int mins)
{
    extern bool Miltime;

    // Display the first digit of the hour
    if (hours < HOURS_PER_DAY) {
        if (Miltime) {
            // Show leading zero, when applicable
            display_num(row, col,
                       (hours < BASE_10) ? 0 : (hours / BASE_10));
        } else {
            // Show no leading zero, i.e., default blank, when applicable
            display_num(row, col,
                       (hours < BASE_10) ? BASE_10 : (hours / BASE_10));
        }
    } else {
        // bad hour
        display_num(row, col, hours);
    }

    // Display the second digit of the hour
    if (hours < HOURS_PER_DAY) {
        display_num(row,
                   (col + HOUR2_OFFSET),
                   (hours < BASE_10) ? hours : (hours % BASE_10));
    } else {
        // bad hour
        display_num(row, (col + HOUR2_OFFSET), hours);
    }

    // Display the colon separating hour and minute
    if (Miltime) {
        // Show no colon
        display_num(row, (col + COLON_OFFSET), BASE_10);
    } else {
        // Show the colon
        display_num(row, (col + COLON_OFFSET), COLON);
    }

    // Display the first digit of the minutes
    if (mins < MINUTES_PER_HOUR) {
        display_num(row, (col + MIN1_OFFSET), (mins / BASE_10));
    } else {
        // bad minute
        display_num(row, (col + MIN1_OFFSET), mins);
    }

    // Display the second digit of the minutes
    if (mins < MINUTES_PER_HOUR) {
        display_num(row, (col + MIN2_OFFSET), (mins % BASE_10));
    } else {
        // bad minute
        display_num(row, (col + MIN2_OFFSET), mins);
    }

}//end display_time

//end display.c
