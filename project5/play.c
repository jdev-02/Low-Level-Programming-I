// ----------------------------------------------------------------------
// File: play.c
//
// Name: Al Shaffer & Paul Clark
//
// Description: This PLAY module defines the rules of game of YAHTZEE,
//     and interacts with the user to roll dice and select where to put
//     a score. The one glaring shortcoming (other than the user
//     interface) is the inability to support the "Joker Rule" where a
//     user can get more than one Yahtzee in a game.
// ----------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <stdbool.h>
#include <ctype.h>
#include <sys/types.h>
#include <unistd.h>
#include "screen.h"
#include "score.h"
#include "play.h"

#define MAX_INPUT       80
#define BASE_10         10
#define CHOOSE_DICE_ROW 1
#define CHOOSE_DICE_COL 1
#define MENU_ROW        15
#define MENU_COL        1

#define NUMBER_OF_DICE       5
#define NUMBER_OF_SIDES      6
#define MAX_ROLLS            3
#define MAX_TURNS            13
#define MAX_FULLHOUSE_MATCH  3
#define MIN_FULLHOUSE_MATCH  2
#define MIN_3KIND_MATCH      3
#define MIN_4KIND_MATCH      4
#define MAX_SMSTRAIGHT_MATCH 2
#define MAX_LGSTRAIGHT_MATCH 1

// Menu selections
#define CHOOSE 'C'
#define ROLL   'R'
#define SCORE  'S'
#define QUIT   'Q'
#define RETURN 'R'


// **************************************************************************
// ****************************  DEFINED TYPES   ****************************
// **************************************************************************

// The structure for tracking a single die
struct die_t {
    unsigned char value;     // The value facing up
    bool          keep;      // Whether to keep or roll
};


// **************************************************************************
// **************************** GLOBAL VARIABLES ****************************
// **************************************************************************

static struct die_t Dice[NUMBER_OF_DICE]; // All the dice
static unsigned int Num_turns;            // # of turns user has taken
static unsigned int Num_rolls;            // # of rolls the user has taken


// **************************************************************************
// *************************** INTERNAL FUNCTIONS ***************************
// **************************************************************************

// ---------------------------------------------------------------------
// Function
//     show_dice
// Inputs
//     none
// Outputs
//     none
// Description
//     This function displays the current state of the dice. For those
//     dice that the user has kept, they are are displayed in black.
//     For those dice that the user wants to re-roll, they are displayed
//     in white.
// ---------------------------------------------------------------------
static void show_dice()
{
    int i;

    // First show the selected dice (if any) not to be rolled
    screen_text_color(BLACK_TEXT);
    for (i = 0; i < NUMBER_OF_DICE; ++i) {
        if (Dice[i].keep) {
            printf("%i ", Dice[i].value);
        }
    }

    // Now show the dice that will be rolled (if any)
    screen_text_color(WHITE_TEXT);
    for (i = 0; i < NUMBER_OF_DICE; ++i) {
        if (!(Dice[i].keep)) {
            printf("%i ", Dice[i].value);
        }
    }
    fflush(stdout);

}//end show_dice


// ---------------------------------------------------------------------
// Function
//     display_menu
// Inputs
//     none
// Outputs
//     none
// Description
//     This function shows the menu to the user.
// ---------------------------------------------------------------------
static void display_menu(void)
{
    screen_cursor(MENU_ROW, MENU_COL);
    printf("Turn %u out of %u\n", Num_turns, MAX_TURNS);
    printf("Roll %u out of %u\n\n", Num_rolls, MAX_ROLLS);
    printf("Menu: %c = Choose the dice to keep or roll\n", CHOOSE);
    printf("      %c = Roll the dice\n", ROLL);
    printf("      %c = Enter a score\n", SCORE);
    printf("      %c = Quit\n", QUIT);

}//end display_menu


// ---------------------------------------------------------------------
// Function
//     how_many_of
// Inputs
//     die
//         This is the die value to be searched for.
// Outputs
//     function result
// Description
//     The function counts the number of die that have the input die
//     "face up", and returns the number found. For example, if the
//     current dice are "3 4 5 5 1", and the input is 5, this function
//     will return 2.
// ---------------------------------------------------------------------
static int how_many_of(const int die)
{
    int count = 0;

    for (int i=0; i < NUMBER_OF_DICE; ++i) {
        if (Dice[i].value == die) {
            ++count;
        }
    }

    return count;

}//end how_many_of


// ---------------------------------------------------------------------
// Function
//     total_of_dice
// Inputs
//     none
// Outputs
//     Function result
// Description
//     This function looks at all the "face up" values of each die and
//     returns their current total.
// ---------------------------------------------------------------------
static int total_of_dice(void)
{
    int score = 0;

    for (int i=0; i < NUMBER_OF_DICE; ++i) {
        score += Dice[i].value;
    }

    return score;

}//end total_of_dice


// ---------------------------------------------------------------------
// Function
//     max_dice_matching
// Inputs
//     none
// Outputs
//     function result
// Description
//     This function determines the highest number of matching die. For
//     example, if the dice were "4 2 1 4 4", the result would be 3.
// ---------------------------------------------------------------------
static int max_dice_matching(void)
{
    int count;
    int max = 0;

    // We'll look at each value and see how many of each we've got
    for (int i=ACES; i <= NUMBER_OF_SIDES; ++i) {
        count = 0;
        for (int j=0; j < NUMBER_OF_DICE; ++j) {
            if (Dice[j].value == i) {
                ++count;
            }
        }
        if (count > max) {
            max = count;
        }
    }

    return max;

}//end max_dice_matching


// ---------------------------------------------------------------------
// Function
//     is_full_house
// Inputs
//     none
// Outputs
//     function result
// Description
//     This function determins whether the current dice values represent
//     a full house (or not), returning true or false.
// ---------------------------------------------------------------------
static bool is_full_house(void)
{
    bool result = false;

    // If we have a 3-of-a-kind, then verify other dice are 2-of-a-kind
    if (max_dice_matching() == MAX_FULLHOUSE_MATCH) {
        // Is there a two-of-a-kind?
        for (int number=1; number <= NUMBER_OF_SIDES; ++number) {
            if (how_many_of(number) == MIN_FULLHOUSE_MATCH) {
                result = true;
                break;
            }
        }
    }

    return result;

}//end is_full_house


// ---------------------------------------------------------------------
// Function
//     assign_score
// Inputs
//     none
// Outputs
//     none
// Description
//     This function prompts the user to select an from the scorecard to
//     apply the current state of the dice. It must determine whether
//     the selected entry matches the current state of the dice. If yes,
//     then the score is determined. If not, then zero is applied to
//     that item.
// ---------------------------------------------------------------------
static void assign_score(void)
{
    int  item;
    int  score;
    int  result = SUCCESS;
    char input[MAX_INPUT];
    char *last_char = NULL;

    while (true) {
        // Repeat the loop until the user enters something other than
        // the RETURN key.
        do {
            // Show the score card and the current dice
            screen_clear();
            score_display();
            printf("\nDice: ");
            show_dice();

            // Prompt the user to pick an item in the score card
            printf("\n\nSelect the item number to place your score: ");
            fflush(stdout);
            fgets(input, MAX_INPUT, stdin);
        } while (input[0] == '\n');

        // get rid of the trailing '\n'
        input[strlen(input)] = '\0';

        // Convert the input string to a number.
        errno = 0;
        item = strtol(input, &last_char, BASE_10);
        if ((last_char == NULL) && (*last_char != '\0')) {
            continue;
        } else if (errno != 0) {
            continue;
        }

        // Now try to enter the appropriate score in the requested place.
        // If, for example, the user selects "Full House", but the roll
        // isn't a Full House, then it's assumed the user wants to put
        // a zero in that spot for a strategic reason. A potential
        // future enhancemet would be to prompt "Are you sure?".
        score = 0;
        if ((item >= ACES) && (item <= SIXES)) {
            for (int i=0; i < NUMBER_OF_DICE; ++i) {
                // The item is in the upper section.
                // Add up the die with that number (if any)
                score = how_many_of(item) * item;
            }
        } else if (item == KIND3) {
            if (max_dice_matching() >= MIN_3KIND_MATCH) {
                score = total_of_dice();
            }
        } else if (item == KIND4) {
            if (max_dice_matching() >= MIN_4KIND_MATCH) {
                score = total_of_dice();
            }
        } else if ((item == FULL_HOUSE) && (is_full_house())) {
            score = SCORE_FULL_HOUSE;
        } else if (item == STRAIGHT_SM) {
            if (max_dice_matching() > MAX_SMSTRAIGHT_MATCH) {
                // A small straight can't have more than two dice matching
                ; // do nothing; score is already zero
            } else if ((how_many_of(THREES) == 0) ||
                       (how_many_of(FOURS) == 0)) {
                // A small straight always has at least a 3 and 4
                ; // do nothing; score is already zero
            } else if ((how_many_of(ACES) > 0) &&
                       (how_many_of(TWOS) > 0)) {
                // A straight with 1, 2, 3, 4
                score = SCORE_STRAIGHT_SM;
            } else if ((how_many_of(TWOS) > 0) &&
                       (how_many_of(FIVES) > 0)) {
                // A straight with 2, 3, 4, 5
                score = SCORE_STRAIGHT_SM;
            } else if ((how_many_of(THREES) > 0) &&
                       (how_many_of(SIXES) > 0)) {
                // A straight with 3, 4, 5, 6
                score = SCORE_STRAIGHT_SM;
            }
        } else if (item == STRAIGHT_LG) {
            // Verify we have a large straight
            if (max_dice_matching() > MAX_LGSTRAIGHT_MATCH) {
                // A large straight has no duplicates
                ; // do nothing; the score is already zero
            } else if ((how_many_of(TWOS) == 0) ||
                       (how_many_of(THREES) == 0) ||
                       (how_many_of(FOURS) == 0)) {
                // A large straight always has 2, 3, 4
                ; // do nothing; the score is already zero
            } else if ((how_many_of(ACES) == 1) &&
                       (how_many_of(FIVES) == 1)) {
                // A large straight of 1, 2, 3, 4, 5
                score = SCORE_STRAIGHT_LG;
            } else if ((how_many_of(TWOS) == 1) &&
                       (how_many_of(SIXES) == 1)) {
                // A large straight of 2, 3, 4, 5, 6
                score = SCORE_STRAIGHT_LG;
            }
        } else if (item == YAHTZEE) {
            if (max_dice_matching() == NUMBER_OF_DICE) {
                score = SCORE_YAHTZEE;
            }
        } else if (item == CHANCE) {
            score = total_of_dice();
        }

        // Try to set the score and leave the loop.
        // Future enhancement: show the reason the request failed.
        result = score_set(item, score);
        if (result == SUCCESS) {
            break;
        }
    }

}//end assign_score


// ---------------------------------------------------------------------
// Function
//     choose_dice
// Inputs
//     none
// Outputs
//     none
// Description
//     The user is presented the current state of the dice, and allows
//     the user to choose which dice to roll, and which to keep. A
//     side-effect is to change the state of the dice.
// ---------------------------------------------------------------------
static void choose_dice(void)
{
    int die;
    char ch;
    bool done = false;

    while (!done) {
        screen_clear();
        screen_cursor(CHOOSE_DICE_ROW, CHOOSE_DICE_COL);

        printf("Die #   Keep   Roll\n");
        printf("-----   ----   ----\n");
        for (int i = 0; i < NUMBER_OF_DICE; ++i) {
            if (Dice[i].keep) {
                printf("    %i   %i\n", i+1, Dice[i].value);
            } else {
                printf("    %i          %i\n", i+1, Dice[i].value);
            }
        }

        // Get what the user wants to switch
        printf("\n\nEnter the die # to change (1 thru 5), or 'R' to return: ");
        ch = getc(stdin);
        if (isdigit(ch)) {
            die = ch - '1';

            // Switch whether to keep or roll
            if ((die >= 0) && (die < NUMBER_OF_DICE)) {
                Dice[die].keep = !(Dice[die].keep);
            }
        } else if (toupper(ch) == RETURN) {
            done = true;
        }
    }

}//end choose_dice


// ---------------------------------------------------------------------
// Function
//     roll_dice
// Inputs
//     none
// Outputs
//     none
// Description
//     This function "rolls the dice" selected by the user by assigning
//     random values to those dice the user didn't keep.
// ---------------------------------------------------------------------
static void roll_dice(void)
{
    for (int i = 0; i < NUMBER_OF_DICE; ++i) {
        if (Dice[i].keep == false) {
            Dice[i].value = (random() % NUMBER_OF_SIDES) + 1;
        }
    }

}//end roll_dice


// ---------------------------------------------------------------------
// Function
//     init_dice
// Inputs
//     none
// Outputs
//     none
// Description
//     This function initializes the dice by "rolling" all the dice and
//     marking them as rollable. This is typically done as the first roll
//     of a turn.
// ---------------------------------------------------------------------
static void init_dice(void)
{
    for (int i = 0; i < NUMBER_OF_DICE; ++i) {
        Dice[i].value = (random() % NUMBER_OF_SIDES) + 1;
        Dice[i].keep  = false;
    }

}//end init_dice


// ************************************************************************
// *************************  EXTERNAL FUNCTIONS **************************
// ************************************************************************
void play_yahtzee(void)
{
    char ch = '\n';

    // Initialize globals
    Num_turns = 1;
    Num_rolls = 1;

    // initialize the PRNG and the dice
    srandom(time(NULL)*getpid());
    init_dice();

    // This loop continues until the user has taken all their turns or
    // the user quits the game.
    while (true) {
        // Is the game over?
        if (Num_turns > MAX_TURNS) {
            break;
        }

        if (Num_rolls == MAX_ROLLS) {
            // The user has used all the rolls for the turn and
            // is forced to enter a score.
            assign_score();
            Num_rolls = 1;
            ++Num_turns;
            init_dice();
        } else {
            while (ch == '\n') {
                // Display the score and the dice
                screen_clear();
                score_display();
                display_menu();
                printf("\nDice (black to keep): ");
                show_dice();

                // Prompt the user for an action to take, and then do it.
                printf("\nAction: ");
                ch = getc(stdin);
            }

            // Do what the user asked
            if (toupper(ch) == QUIT) {
                break;
            } else if (toupper(ch) == CHOOSE) {
                choose_dice();
            } else if (toupper(ch) == ROLL) {
                roll_dice();
                ++Num_rolls;
            } else if (toupper(ch) == SCORE) {
                assign_score();
                Num_rolls = 1;
                ++Num_turns;
                init_dice();
            } else {
                // Bad selection. Do nothing and loop back to prompt again
                ;
            }
        }

        // Reset the selection to nothing
        ch = '\n';
    }

}//end play_yahtzee

// end play.c
