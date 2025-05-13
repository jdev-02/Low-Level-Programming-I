// ----------------------------------------------------------
// File: screen.h
//
// Name: Paul Clark
//
// Description:
//     This is the header file for the SCREEN module.
// ----------------------------------------------------------

#ifndef SCREEN_H
#define SCREEN_H

#define BLACK_TEXT 30
#define WHITE_TEXT 97

extern void screen_init(void);
extern void screen_reset(void);
extern void screen_clear(void);
extern void screen_cursor(const int row, const int col);
extern void screen_text_color(const int color);

#endif // SCREEN_H
