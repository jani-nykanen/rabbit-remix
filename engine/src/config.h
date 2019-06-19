//
// Configuration
// (c) 2019 Jani Nykänen
//


#ifndef __CONFIG__
#define __CONFIG__

#include "types.h"


#define CONF_MAX_PARAM_COUNT 32


// Config type
typedef struct {

    KeyValuePair params [CONF_MAX_PARAM_COUNT];
    int paramCount;

} Config;

// Create a configuration
Config create_config();

// Add a param
int conf_add_param(Config* c, const char* key, const char* value);

// Get a param
char* conf_get_param(Config* c, const char* key, const char* def);

// Get a param in integer format
int conf_get_param_int(Config* c, const char* key, int def);

// Parse a text file
int conf_parse_text_file(Config* c, const char* path);

#endif // __CONFIG__
