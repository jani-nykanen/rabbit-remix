//
// Just MD5, but in different file so
// CURL does not make us include OpenSSL
// (c) 2019 Jani Nykänen
//

#ifndef __CRYPT__
#define __CRYPT__


// String to MD5, hex format
void md5(char* in, char* out, int inLen);


#endif // __CRYPT__
