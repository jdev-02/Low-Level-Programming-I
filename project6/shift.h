// -------------------------------------------------------------------
// File: shift.h
//
// Name: Jonathan Goohs
//
// Description: This is the header file for the shift module of the
//     caesar shift program.
// -------------------------------------------------------------------
#include <stddef.h>

//header guard
#ifndef SHIFT_H
#define SHIFT_H

extern void encrypt_file(char *buffer, size_t length, unsigned int shift);
extern void decrypt_file(char *buffer, size_t length, unsigned int shift);

#endif
//end shift.h