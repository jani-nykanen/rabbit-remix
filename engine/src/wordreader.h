//
// Word reader
// (c) 2019 Jani Nykänen
//

#ifndef __WORD_READER__
#define __WORD_READER__

#include <stdio.h>
#include <stdbool.h>

#define WR_WORD_LENGTH 128

// Word reader
typedef struct 
{
    FILE* f;
    char word [WR_WORD_LENGTH];
    int wordLength;

} WordReader;

// Create a word reader
WordReader* create_word_reader(const char* path);

// Read the next word
int wr_read_next(WordReader* wr);

// Destroy
void dispose_word_reader(WordReader* wr);

#endif // __WORD_READER__
