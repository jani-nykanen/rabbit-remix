#include "err.h"

#include <stdlib.h>
#include <stdio.h>

// Error buffer
static char errBuffer [ERR_MAX_LENGTH];
// Does an error exist
static bool hasError =false;


// Initialize error handling
void err_init() {

    hasError = false;
}


// Throw an error
void err_throw_no_param(const char* msg) {

    snprintf(errBuffer, ERR_MAX_LENGTH, "%s", msg);
    hasError = true;
}
void err_throw_param_1(const char* msg, const char* param) {

    snprintf(errBuffer, ERR_MAX_LENGTH, "%s%s", msg, param);
    hasError = true;
}


// Get an error. Returns NULL
// if does not exist
char* get_error() {

    char* ret = hasError ? errBuffer : NULL;
    hasError = false;

    return ret;
}
