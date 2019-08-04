#include "crypt.h"

#include "lib/md5.h"

#include <stdio.h>


// String to MD5, hex format
void md5(char* in, char* out, int inLen) {

    unsigned char digest[1024];
    _MD5_CTX context;

    _MD5_Init(&context);
    _MD5_Update(&context, in, inLen);
    _MD5_Final(digest, &context);

    // Turn to hex format
    for(int i = 0; i < 16; ++i)
        sprintf(&out[i*2], "%02x", (unsigned int)digest[i]);
}
