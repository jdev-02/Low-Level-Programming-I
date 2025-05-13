// ----------------------------------------------------------------------
// File: score.c
//
// Name: Al Shaffer & Paul Clark & Jonathan Goohs
//
// Description: This is the implementation of the SCORE module of the
//     YAHTZEE game. It keeps track of the score card, enters new
//     scores as requested, and displays the score as requested.
//
//Resources:
//1. man console_codes
//2. geeksforgeeks.org/printf-in-c/
// ----------------------------------------------------------------------
#include <stdio.h>
#include <stdbool.h>
#include "score.h"
#include "screen.h"

#define NUMBER_OF_ENTRIES      13  // Does not count the subtotals and totals
#define LEFT_SECTION_END        6
#define RIGHT_SECTION_END      13
#define NOT_SCORED              0
#define LEFT_BONUS_SUBTOTAL    63
#define BONUS_VALUE            35
#define LEFT_SECTION_ROW        0
#define LEFT_SECTION_COL       11
#define YAHTZEE_ROW             0
#define YAHTZEE_COL            38
#define RIGHT_SECTION_ROW       0
#define RIGHT_SECTION_COL      62
#define RIGHT_ITEM_COL         50
#define EQ_SCORE_D_ROW         10
#define TOTAL_L_SCORE_D_ROW    11
#define TOTAL_R_SCORE_D_ROW    12
#define GRAND_T_SCORE_D_ROW    13
#define SCORE_DISPLAY_COL      53
#define UPPER_SECTION_ROW       1
#define UPPER_SECTION_COL       2
#define UPPER_SECTION_ITEM_COL  3
#define LOWER_SECTION_ROW      12
#define LOWER_SECTION_COL       0
#define LOWER_SECTION_ITEM_COL  3
#define EQ_LOWER_S_FINAL_ROW   20
#define TOTAL_UP_FINAL_ROW     21
#define TOTAL_LOW_FINAL_ROW    22
#define GRAND_T_FINAL_ROW      23
#define LOWER_SECTION_FINAL_COL 4
#define FINAL_CURSOR_ROW       24
#define FINAL_CURSOR_COL        4


// **************************************************************************
// ****************************  DEFINED TYPES   ****************************
// **************************************************************************

// An entry in a scorecard
struct entry_t {
    unsigned int value;
    bool         used;
};

// **************************************************************************
// **************************** GLOBAL VARIABLES ****************************
// **************************************************************************

// The Scorecard
static struct entry_t Score[NUMBER_OF_ENTRIES+1]; // row 0 is not used

// The Entry Names in a scorecard
static char *Entry_names[] = {
    "******** NOT USED *********",
    "Aces     (total of all 1's)",   // Keep these col's all the same widths
    "Twos     (total of all 2's)",
    "Threes   (total of all 3's)",
    "Fours    (total of all 4's)",
    "Fives    (total of all 5's)",
    "Sixes    (total of all 6's)",
    "3 of a Kind  (add all dice)",
    "4 of a Kind  (add all dice)",
    "Full House   (score 25)    ",
    "Sm. Straight (score 30)    ",
    "Lg. Straight (score 40)    ",
    "YAHTZEE      (score 50)    ",
    "Chance       (add all dice)"
};

static int left_score = NOT_SCORED; //initialize to zero and add to it based off of loop value
static int right_score = NOT_SCORED; //initialize to zero and add to it based off of loop value
static int bonus = NOT_SCORED;
static int total_score = NOT_SCORED; //need to save values for added scores for printing
static int grand_total = NOT_SCORED;

// **************************************************************************
// *************************** EXTERNAL FUNCTIONS ***************************
// **************************************************************************

// ---------------------------------------------------------------------
// Function
//     score_reset
// Inputs
//     none
// Outputs
//     none
// Description
//     Initializes all the scorecard entries to zero and unused.
// ---------------------------------------------------------------------
void score_reset(void)
{
    for (int i =1;i<NUMBER_OF_ENTRIES;i++) {
        Score[i].value = 0;
        Score[i].used = false;
    }
}//end score_reset


// ---------------------------------------------------------------------
// Function
//     score_display
// Inputs
//     none
// Outputs
//     none
// Description
//     Displays the current scorecard using the entire screen.
// ---------------------------------------------------------------------
void score_display(void)
{
    //all individual scores, total scores, grand total, and the bonus score (which if theres more than 63 you get bonus score)
    
    total_score = left_score + right_score; //need to save values for added scores for printing
    grand_total = total_score + bonus; // per game rules

    screen_cursor(LEFT_SECTION_ROW,LEFT_SECTION_COL);
    screen_text_color(WHITE_TEXT);
    printf("LEFT SECTION\n\n\n");
    //print aces through 6
    for (int i=1;i<=LEFT_SECTION_END;i++){
        if (Score[i].used == false){
            printf("%d %s %2d\n", i, Entry_names[i], NOT_SCORED);
        } else {     //the used bool is true
            printf("%s %s %2d\n", " ", Entry_names[i], Score[i].value);
        }
    }
    printf("  ========================== ===\n");
    printf("  TOTAL SCORE\t\t     %3d\n", left_score);

    //bonus - assigned 35 points whenever subtotal of left is >= 63
    if (left_score >= LEFT_BONUS_SUBTOTAL) {
        bonus = BONUS_VALUE;
        printf("  BONUS\t\t\t     %3d\n", bonus);
    } else {
        printf("  BONUS\t\t\t     %3d\n", bonus);
    }

    printf("  TOTAL_LEFT\t\t     %3d\n", left_score);

    //middle section
    screen_cursor(YAHTZEE_ROW,YAHTZEE_COL);
    printf("YAHTZEE!");

    //right section
    screen_cursor(RIGHT_SECTION_ROW,RIGHT_SECTION_COL);
    printf("RIGHT SECTION\n");
    int RIGHT_ITEM_ROW = 3;
    //print 3 of a kind through chance
    for (int i=7;i<=RIGHT_SECTION_END;i++){
        screen_cursor(RIGHT_ITEM_ROW++,RIGHT_ITEM_COL);
        if (Score[i].used == false){
            printf("%2d %s %2d\n", i, Entry_names[i], NOT_SCORED);
        } else {     //the used bool is true
                printf("%s %s %2d\n", "  ", Entry_names[i], Score[i].value);
            }
        }
    //display totals
    screen_cursor(EQ_SCORE_D_ROW,SCORE_DISPLAY_COL);
    printf("========================== ===\n");
    screen_cursor(TOTAL_L_SCORE_D_ROW,SCORE_DISPLAY_COL);
    printf("TOTAL LEFT\t\t\t%2d\n", left_score);
    screen_cursor(TOTAL_R_SCORE_D_ROW,SCORE_DISPLAY_COL);
    printf("TOTAL RIGHT\t\t\t%2d\n", right_score);
    screen_cursor(GRAND_T_SCORE_D_ROW,SCORE_DISPLAY_COL);
    printf("GRAND TOTAL\t\t\t%2d\n", grand_total);

}//end score_display


// ---------------------------------------------------------------------
// Function
//     score_display_final
// Inputs
//     none
// Outputs
//     none
// Description
//     Displays the final scorecard in a simple format.
// ---------------------------------------------------------------------
void score_display_final(void)
{
    // Add your code here
    //when user hits q or the game is finished, the scorecard will be displayed
    //upper section, lower section, and scores
    screen_clear();
    screen_cursor(0,0);
    total_score = left_score + right_score; //need to save values for added scores for printing
    grand_total = total_score  + bonus;

    printf("UPPER SECTION\n");
    screen_cursor(UPPER_SECTION_ROW,UPPER_SECTION_COL);
    int UPPER_SECTION_ITEM = 2;
    for (int i=1;i<=LEFT_SECTION_END;i++){      //start from one to avoid first item
        screen_cursor(UPPER_SECTION_ITEM++,UPPER_SECTION_ITEM_COL);
        if (Score[i].used == false){
            printf("%s %s %2d\n", "", Entry_names[i], NOT_SCORED);
        } else {
            int section_value = Score[i].value;
            printf("%s %s %2d\n", "", Entry_names[i], section_value);
        }
    }
    printf("   ========================== ===\n");
    printf("   TOTAL SCORE\t\t      %3d\n", total_score);

    //bonus - assigned 35points whenever subtotal of left is >= 63
    if (left_score >= LEFT_BONUS_SUBTOTAL) {
        bonus = BONUS_VALUE;
        printf("   BONUS\t\t       %2d\n", bonus);
    } else {
        printf("   BONUS\t\t       %2d\n", bonus);
    }

    printf("   TOTAL_UPPER\t\t      %3d\n", left_score);

    //end upper section
    screen_cursor(LOWER_SECTION_ROW,LOWER_SECTION_COL);
    printf("LOWER SECTION\n");
    int LOWER_SECTION_ITEM_ROW = 13;

    //print 3 of a kind through chance
    for (int i=7;i<=RIGHT_SECTION_END;i++){
        screen_cursor(LOWER_SECTION_ITEM_ROW++, LOWER_SECTION_ITEM_COL);
        if (Score[i].used == false){
            printf("%s %s %2d\n", "", Entry_names[i], NOT_SCORED);
        } else {        //the used bool is true
                int section_value = Score[i].value;
                printf("%s %s %2d\n", "", Entry_names[i], section_value);
        }
    }
    screen_cursor(EQ_LOWER_S_FINAL_ROW,LOWER_SECTION_FINAL_COL);
    printf("========================== ===\n");
    screen_cursor(TOTAL_UP_FINAL_ROW,LOWER_SECTION_FINAL_COL);
    printf("TOTAL UPPER\t\t      %3d\n", left_score);
    screen_cursor(TOTAL_LOW_FINAL_ROW,LOWER_SECTION_FINAL_COL);
    printf("TOTAL LOWER\t\t      %3d\n", right_score);
    screen_cursor(GRAND_T_FINAL_ROW,LOWER_SECTION_FINAL_COL);
    printf("GRAND TOTAL\t\t      %3d\n", grand_total);

    screen_cursor(FINAL_CURSOR_ROW,FINAL_CURSOR_COL);
}//end score_display_final


// ---------------------------------------------------------------------
// Function
//     score_set
// Inputs
//     item
//         This is the line in the scorecard to put the score.
//     score
//         This is the score to be put on the selected line.
// Outputs
//     function result
// Description
//     This function updates the line on the scorecard with the given
//     score. The item must not already be used by a previous entry, or
//     the function returns a non-SUCCESS and no change to the card
//     happens. The given item must also exist, or the function returns
//     a non-SUCCESS and no change to the card happens. Otherwise, the
//     requested change occurs, and a SUCCESS is returned.
// ---------------------------------------------------------------------
int score_set(const int item, const int score)
{
    //requires user input, so must error check
    //input will be 
    //check if items status is used (set to true)
    //refer to get input from last project

    //check if user item index is valid
    if (item < 1 || item >= NUMBER_OF_ENTRIES) {
        return !SUCCESS;
    }
    //check if score is negative
    if (score < 0) {
        return !SUCCESS;
    }
    //check if score entry has already been entered by user
    if (Score[item].used == true) {
        return !SUCCESS;
    }
    //update scorecard with entry from user
    Score[item].value = score;
    Score[item].used = true;

    //update section totals
    if (item >= 1 && item <= 6) {
        left_score += score;
        if (left_score >= LEFT_BONUS_SUBTOTAL){
            bonus = BONUS_VALUE;
        } else {
            bonus = NOT_SCORED;
        }
    } else {        //the number is 7-13
        right_score += score;
    }

    //update final and grand total 
    total_score = left_score + right_score;
    grand_total = left_score + right_score + bonus;

    return SUCCESS;
}//end score_set
