// -------------------------------------------------------------------
// File: score.h
//
// Name: Paul Clark
//
// Description: This is the header file for the SCORE module of the
//     YAHTZEE game.
// -------------------------------------------------------------------
#ifndef SCORE_H
#define SCORE_H

#define ACES              1
#define TWOS              2
#define THREES            3
#define FOURS             4
#define FIVES             5
#define SIXES             6
#define KIND3             7
#define KIND4             8
#define FULL_HOUSE        9
#define STRAIGHT_SM       10
#define STRAIGHT_LG       11
#define YAHTZEE           12
#define CHANCE            13

#define SCORE_FULL_HOUSE  25
#define SCORE_STRAIGHT_SM 30
#define SCORE_STRAIGHT_LG 40
#define SCORE_YAHTZEE     50

#define SUCCESS           0

extern int  score_set(const int item, const int score);
extern void score_reset(void);
extern void score_display(void);
extern void score_display_final(void);

#endif
