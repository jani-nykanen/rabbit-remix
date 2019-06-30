#include "wordreader.h"

#include "err.h"

#include <stdlib.h>


// Create a word reader
WordReader* create_word_reader(const char* path) {

    // Allocate memory
    WordReader* wr = (WordReader*)malloc(sizeof(WordReader));
    if (wr == NULL) {

        ERR_MEM_ALLOC;
        return NULL;
    }

    // Open the file
    wr->f = fopen(path, "r");
    if (wr->f == NULL) {

        err_throw_param_1("Failed to open a file in ", path);
        free(wr);
        return NULL;
    }

    wr->word[0] = '\0';
    wr->wordLength = 0;

    return wr;
}


// Read the next word
int wr_read_next(WordReader* wr) {

    // Read the next character
    char c;
    bool comment = false;
    bool quotation = false;
    bool whitespace = false;
    wr->wordLength = 0;

    while(true) {

        // Get next character
        c = fgetc(wr->f);
        whitespace = (c == ' ' || c == '\t' || c == '\n');
        if (c == EOF) {

            // Null-terminate
            if (wr->wordLength < WR_WORD_LENGTH-1) {

                wr->word[wr->wordLength] = '\0';
            }

            return 0;
        }
        else if (comment) {
            
            if (c == '\n')
                comment = false;

            continue;
        }
        // If comment, wait until a newline
        else if (c == '#') {

            comment = true;
        }
        // Quatation?
        else if (c == '"') {

            quotation = !quotation;
            if (!quotation && wr->wordLength > 0)
                break;
            
        }
        // Whitespace, stop
        else if (whitespace && !quotation) {
            
            if (wr->wordLength > 0) {
                
                // Null-terminate the string
                wr->word[wr->wordLength ++ ] = '\0';
                break;
            }
        }
        // Other character
        else {

            wr->word[wr->wordLength ++ ] = c;
            if (wr->wordLength == WR_WORD_LENGTH-1) {

                wr->word[wr->wordLength] = '\0';
                break;
            }
        }
    }

    // Null-terminate
    if (wr->wordLength < WR_WORD_LENGTH-1) {

        wr->word[wr->wordLength] = '\0';
    }

    return 1;
}


// Destroy
void dispose_word_reader(WordReader* wr) {

    fclose(wr->f);
    free(wr);
}

