// ----------------------------------------------------------------------
// File: shift.c
//
// Name: Jonathan Goohs
//
// Description: This is the shift program for a simple caesar cipher shift program.
//
// Syntax: ./shift -e|-d key input output 
//
//Resources: 
// size_t man page
// www.geeksforgeeks.org/c-ascii-value/
// www.scaler.com/topics/caesar-cipher-program-in-c/
// ----------------------------------------------------------------------

//Headers/Libraries
#include "shift.h"
#include <string.h>
#include <stddef.h>
#include <stdio.h>

//Constants
#define LETTER_MOD   26
#define DIGIT_MOD    10
#define LETTERS      26
#define DIGITS       10
#define MIN_DIGIT    '0'
#define MAX_DIGIT    '9'
#define MIN_LETTER_C 'A'
#define MAX_LETTER_C 'Z'
#define MIN_LETTER_L 'a'
#define MAX_LETTER_L 'z'
///Function:
//  encrypt_file
//inputs:
//  char *buffer - the buffer content of the file
//  size_t length - size of the file to be encrypted
//  unsigned int shift - the value of which to shift the content of the input file by forward (encrypting)
//outputs:
//  
//description:
//  1. check if the buffer is not null, then proceed with encryption logic
//  2. loop through the length of the file and apply the shift value to each letter by adding it and then printing back out
//  3. small letters encryption is (ch-'a' + key) mod 26 + 'a', capital letters (ch-'A' + key) mod 26 + 'A', digits (ch-'0' + key) mod key + '0'
void encrypt_file(char *buffer, size_t length, unsigned int shift){
    if (buffer == NULL) {
        return;
    }       //else proceed with encryption

    for (size_t i = 0; i < length; i++) {
        char c = buffer[i];

        if (c >= MIN_LETTER_C && c <= MAX_LETTER_C) {
            //shifting uppercase formula
            buffer[i] = ((c - MIN_LETTER_C + shift) % LETTER_MOD) + MIN_LETTER_C;
        } else if (c >= MIN_LETTER_L && c <= MAX_LETTER_L) {
            //shifting lowercase formula
            buffer[i] = ((c - MIN_LETTER_L + shift) % LETTER_MOD) + MIN_LETTER_L;
        } else if (c >= MIN_DIGIT && c <= MAX_DIGIT) {
            //shifting for digits
            buffer[i] = ((c - MIN_DIGIT + shift) % DIGIT_MOD) + MIN_DIGIT;
        }
    }
}

///Function:
//  decrypt_file
//inputs:
//  char *buffer - the buffer content of the file
//  size_t length - size of the file to be decrypted
//  unsigned int shift - the value of which to shift the content of the input file by backwards (decrypting)
//outputs:
//  result - the char *of decrypted text to be written back to the output file
//description:
//  1. for letters - decryption is just encryption with (26-key)%26
//  2. for digits - decryption is just encryption with (10-key)%10

void decrypt_file(char *buffer, size_t length, unsigned int shift){
    
    unsigned int letter_d_shift = (LETTERS - (shift % LETTER_MOD)) % LETTER_MOD;
    unsigned int digit_d_shift = (DIGITS - (shift % DIGIT_MOD)) % DIGIT_MOD;
    
    if (buffer == NULL) {
        return;
    }       //else proceed with decyption

    
    for (size_t i = 0; i < length; i++) {
        char c = buffer[i];

        if (c >= MIN_LETTER_C && c <= MAX_LETTER_C) {
            //shifting uppercase formula backwards
            buffer[i] = ((c - MIN_LETTER_C + letter_d_shift) % LETTER_MOD) + MIN_LETTER_C;
        } else if (c >= MIN_LETTER_L && c <= MAX_LETTER_L) {
            //shifting lowercase formula
            buffer[i] = ((c - MIN_LETTER_L + letter_d_shift) % LETTER_MOD) + MIN_LETTER_L;
        } else if  (c >= MIN_DIGIT && c <= MAX_DIGIT) {
            //shifting for digits
            buffer[i] = ((c - MIN_DIGIT + digit_d_shift) % DIGIT_MOD) + MIN_DIGIT;
        }
    }
}


//end shift.c