#include "leaderboard.h"

#include <openssl/md5.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <SDL2/SDL.h>

#include <curl/curl.h>

// Server & key
#define SERVER "https://game-leaderboards.000webhostapp.com/rabbit-remix"
#define KEY "DUMMYKEY"

// Return buffer size
#define RET_BUFFER_SIZE 1024
#define ADDR_LEN_MAX 256 

// Handle
static CURL* handle;
// Address
static char address [ADDR_LEN_MAX];

// Return buffer
static char retBuffer[RET_BUFFER_SIZE];
// Word buffer
static char wbuffer[RET_BUFFER_SIZE +1];
// Buffer pointer
static size_t bufptr;

// MD5 context
static MD5_CTX md5;


// Get next word
static bool get_next_word() {

    wbuffer[0] = '\0';
    int p = 0;

    // End of the buffer reached, nothing
    // to return
    if (bufptr > RET_BUFFER_SIZE ||
        retBuffer[bufptr] == '\0')
        return 0;

    while(retBuffer[bufptr] != '|' && 
        bufptr < RET_BUFFER_SIZE &&
        retBuffer[bufptr] != '\0') {

        wbuffer[p ++] = retBuffer[bufptr ++];
    }
    wbuffer[p] = '\0';
    ++ bufptr; 

    return true;
}


// Check success (aka first word)
static bool check_success() {

    bufptr = 0;
    get_next_word();
    return strcmp(wbuffer, "true") == 0;
}


// Parse entries
static void parse_entries(Leaderboard* lb) {

    int c = 0;
    int p = 0;

    while(get_next_word()) {

        // Name
        if ( (c ++) % 2 == 0) {

            snprintf(lb->entries[p].name, LB_NAME_LENGTH, "%s", wbuffer);
        }
        // Score
        else {

            lb->entries[p].score = (int)strtol(wbuffer, NULL, 10);
            ++ p;
        }
    }
}


// Receive
static size_t receive(void* data, size_t size, size_t mem, void* user) {

    // Data in characters
    char* chr = (char*)data;
    size_t len = size * mem;

    // Copy received data to the buffer
    int i = 0;
    for(; i < len; ++ i) {

        // TODO: Better error handling
        if(bufptr >= RET_BUFFER_SIZE) {

            printf("Leaderboard error: Buffer overflow.\n");
            return 0;
        }

        retBuffer[bufptr ++] = chr[i];
    }

    // printf("Received: %s\n", retBuffer);

    return size * mem;
}


// Initialize CURL
static int init_CURL(char* addr) {

     // Create handle
    handle = curl_easy_init();
    if(handle == NULL) {

        printf("Leaderboard error: Could not create a CURL handle.\n");
        return 1;
    }

    // Set receiver
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, receive);

    // Copy address
    snprintf(address, ADDR_LEN_MAX, "%s", addr);

    // Also init MD5 here
    MD5_Init(&md5);

    return 0;
}


// Send a request
static int send_request(const char* req) {

    // Set URL
    char fullAddress [1024];
    snprintf(fullAddress, 1024, "%s/?%s", address, req);
    curl_easy_setopt(handle, CURLOPT_URL, fullAddress);

   // printf("Sent: %s\n", fullAddress);

    // Clear buffer
    bufptr = 0;
    memset(retBuffer,'\0', RET_BUFFER_SIZE);

    // Send request & handle data
    char errCode[16];
    CURLcode success = curl_easy_perform(handle);
    if(success != CURLE_OK) {

        snprintf(errCode, 16, "%d", (int)success);
        printf("Leaderboard error: HTTP request failed with code %s.\n", errCode);

        return 1;
    }

    return 0;
}


// Initialize global leaderboard stuff
int init_global_leaderboard() {

    return init_CURL((char*)SERVER);
}


// Create a leaderboard
Leaderboard create_leaderboard() {

    Leaderboard lb;
    
    // Set, for testing purposes
    int i;
    for (i = 0; i < LB_ENTRY_MAX; ++ i) {

        snprintf(lb.entries[i].name, LB_NAME_LENGTH, "DEFAULT");
        lb.entries[i].score = 10000 - i*1000;
    }

    return lb;
}


// Fetch scores
int lb_fetch_scores(Leaderboard* lb) {

    // Return something, for testing purposes
    int i;
    for (i = 0; i < LB_ENTRY_MAX; ++ i) {

        snprintf(lb->entries[i].name, LB_NAME_LENGTH, "DEFAULT");
        lb->entries[i].score = 10000 - i*1000;
    }

    // Send request
    if(send_request("&mode=get") == 1) {

        return 1;
    }

    // Check success
    if (!check_success()) {

        return 1;
    }
    // Parse entries
    parse_entries(lb);

    return 0;
}


// Add a score
int lb_add_score(Leaderboard* lb, char* name, int score) {

    char check [1024];
    snprintf(check, 1024, "%s%d", KEY, score);
    // printf("%s\n", check);

    unsigned char digest[1024];
    MD5_CTX context;

    MD5_Init(&context);
    MD5_Update(&context, check, strlen(check));
    MD5_Final(digest, &context);

    // Turn to hex format
    char out[1024];
    for(int i = 0; i < 16; ++i)
        sprintf(&out[i*2], "%02x", (unsigned int)digest[i]);
    // printf("%s\n", out);

    char send [1024];
    snprintf(send, 1024, "&mode=set&name=%s&score=%d&check=%s", name, score, out);

    // Send request
    if (send_request(send) == 1) {

        return 1;
    }
    // Check success
    if (!check_success()) {

        return 1;
    }
    // Parse entries
    parse_entries(lb);

    return 0;
}
