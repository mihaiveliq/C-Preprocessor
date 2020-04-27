#include "hash-map.h"

typedef struct {
    int got_input; 
    int got_output;
    char *out_name;
    char *in_name;
    hashmap_t *map;
    storage_t *dir_strg;
    storage_t *key_strg;
} input_t;
