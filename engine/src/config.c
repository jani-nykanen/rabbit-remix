#include "config.h"

#include "err.h"
#include "wordreader.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h> 


// Create a configuration
Config create_config() {

    Config c;
    c.paramCount = 0;

    return c;
}


// Add a param
int conf_add_param(Config* c, const char* key, const char* value) {

    if (c->paramCount >= CONF_MAX_PARAM_COUNT) {

        err_throw_no_param("Configuration object is full.");
        return -1;
    }   

    // Store the param
    KeyValuePair kv;
    snprintf(kv.key, KVPAIR_KEY_LENGTH, "%s", key);
    snprintf(kv.value, KVPAIR_VALUE_LENGTH, "%s", value);
    c->params[c->paramCount] = kv;

    ++ c->paramCount;

    return 0;
}


// Get a param
char* conf_get_param(Config *c, const char* key, const char* def) {

    int i = 0;
    for(; i < c->paramCount; ++ i) {

        if (strcmp(c->params[i].key, key) == 0)
            return c->params[i].value;
    }

    return (char*)def;
}


// Get a param in integer format
int conf_get_param_int(Config* c, const char* key, int def) {

    char* str = conf_get_param(c, key, NULL);
    if (str == NULL) return def;

    return (int)strtol(str, NULL, 10);
}


// Parse a text file
int conf_parse_text_file(Config* c, const char* path) {

    // Create a word reader
    WordReader* wr = create_word_reader(path);
    if (wr == NULL) {

        return -1;
    }

    // Read every word
    char key[KVPAIR_KEY_LENGTH];
    char value[KVPAIR_VALUE_LENGTH];
    bool m = true;
    while(wr_read_next(wr)) {

        // Store params to the configuration
        if (wr->wordLength > 0) {

            snprintf(
                m ? key : value, m ? KVPAIR_KEY_LENGTH : KVPAIR_VALUE_LENGTH,
                "%s", wr->word
            );
            m = !m;

            if (m)
                conf_add_param(c, key, value);
        }
    }

    // Close & destroy
    dispose_word_reader(wr);

    return 0;
}
